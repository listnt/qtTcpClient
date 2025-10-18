#include "tcpserver.h"
#include <QString>

TcpServer::TcpServer() {}
TcpServer::~TcpServer() {}

void TcpServer::initSocket() {
  this->_sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (this->_sockfd == -1) {
    throw std::runtime_error("Socket creation failed");
  }

  const int yes = 1;

  // Enable reuse
  if (setsockopt(this->_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) ==
      -1)
    throw std::runtime_error("setsockopt(SO_REUSEADDR)");

  // Enable keep alive
  if (setsockopt(this->_sockfd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int)) ==
      -1)
    throw std::runtime_error("setsockopt(SO_KEEPALIVE)");

  // Set TCP keepalive parameters (Linux only)
  int keepIdle = 60;     // 60 seconds before sending keepalive probes
  int keepInterval = 10; // 10 seconds between probes
  int keepCount = 5;     // 5 failed probes before dropping connection

  if (setsockopt(this->_sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle,
                 sizeof(keepIdle)) < 0)
    throw std::runtime_error("setsockopt(TCP_KEEPIDLE)");

  if (setsockopt(this->_sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval,
                 sizeof(keepInterval)) < 0)
    throw std::runtime_error("setsockopt(TCP_KEEPINTVL)");

  if (setsockopt(this->_sockfd, IPPROTO_TCP, TCP_KEEPCNT, &keepCount,
                 sizeof(keepCount)) < 0)
    throw std::runtime_error("setsockopt(TCP_KEEPCNT)");
}

void TcpServer::initAddress(int port) {
  this->_server_addr.sin_family = AF_INET;
  this->_server_addr.sin_addr.s_addr = INADDR_ANY;
  this->_server_addr.sin_port = htons(port);

  const int bind_res =
      bind(this->_sockfd, (struct sockaddr *)&this->_server_addr,
           sizeof(this->_server_addr));

  if (bind_res == -1) {
    throw std::runtime_error("Bind failed");
  }
}

void TcpServer::initServer(int port, int max_clients) {
  this->port = port;
  this->max_clients = max_clients;

  this->initSocket();
  this->initAddress(port);
  this->newClientHandler = [this](int clientfd) {
    this->_clientConnected(clientfd);
  };

  this->_clients_fds.reserve(max_clients);

  const int listen_res = listen(this->_sockfd, max_clients);
  if (listen_res == -1) {
    throw std::runtime_error("Listen failed");
  }
}

std::pair<bool, std::string> TcpServer::acceptClient(int timeout) {
  auto res = waitForClient(timeout);
  if (!res.first) {
    return res;
  }

  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);

  int client_fd =
      accept(this->_sockfd, (struct sockaddr *)&client_addr, &client_addr_len);

  if (client_fd == -1) {
    return {false, std::string(strerror(errno))};
  }

  this->_mu.lock();
  this->_clients_fds.push_back(client_fd);
  this->_mu.unlock();

  if (this->newClientHandler != nullptr) {
    this->newClientHandler(client_fd);
  }

  return {true, ""};
}

std::pair<bool, std::string> TcpServer::waitForClient(int timeout) {
  struct pollfd pfd;
  pfd.fd = this->_sockfd;
  pfd.events = POLLIN;

  int res = poll(&pfd, 1, timeout * 1000);

  if (res == -1) {
    return {false, std::string(strerror(errno))};
  }

  if (res == 0) {
    return {false, "Timeout"};
  }

  return {true, "New client"};
}

void TcpServer::setRecieveFunc(std::function<void(int, std::string)> func) {
  this->_recieveFunc = func;
}

void TcpServer::setNewClientHandler(std::function<void(int clientfd)> func) {
  this->newClientHandler = func;
}

std::pair<bool, std::string> TcpServer::sendMsgToClient(int clientfd,
                                                        std::string msg) {
  int res = send(clientfd, msg.c_str(), msg.length(), 0);

  if (res == -1) {
    return {false, std::string(strerror(errno))};
  }

  return {true, ""};
}

void TcpServer::sendMsgToAllClient(std::string msg) {
  for (auto client_fd : this->_clients_fds) {
    send(client_fd, msg.c_str(), msg.length(), MSG_DONTWAIT);
  }
}

void TcpServer::startListening() {
  this->_acceptClientThread = new std::thread([this]() {
    while (true) {
      this->acceptClient(30);
    }
  });

  char buffer[1024];
  std::vector<struct pollfd> pollfds(this->max_clients);

  while (true) {
    for (int i = 0; i < this->_clients_fds.size(); i++) {
      pollfds[i].fd = this->_clients_fds[i];
      pollfds[i].events = POLLIN;
    }

    int res = poll(&pollfds[0], this->_clients_fds.size(), 1);
    if (res == -1) {
      throw std::runtime_error("Poll failed");
    }

    if (res == 0) {
      continue;
    }

    for (int i = 0; i < this->_clients_fds.size(); ++i) {
      if (pollfds[i].revents & POLLIN) {
        // Data available to read
        ssize_t bytes = recv(pollfds[i].fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
          close(pollfds[i].fd);

          pollfds[i] = pollfds[this->_clients_fds.size() - 1];
          this->_clients_fds[i] =
              this->_clients_fds[this->_clients_fds.size() - 1];
          this->_clients_fds.pop_back();
          i--;
        } else {
          buffer[bytes] = '\0';
          this->_recieveFunc(pollfds[i].fd, std::string(buffer));
        }
      }
    }
  }
}

void TcpServer::stopListening() {
  zap::info << "msg" << "Stopping server";
  this->_acceptClientThread->join();
}

void TcpServer::_clientConnected(int clientfd) {
  QString msg =
      QString::asprintf("status=%s; clientfd=%d; currentClients=%zu;",
                        "connected", clientfd, this->_clients_fds.size());

  // throw away any errors
  this->sendMsgToClient(clientfd, msg.toStdString());
}
