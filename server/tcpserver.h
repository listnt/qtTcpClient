
#include <arpa/inet.h>
#include <cstdint>
#include <cstdio>
#include <cstring> // For strerror
#include <errno.h>
#include <format>
#include <functional>
#include <iostream>
#include <unistd.h>
#include <mutex>
#include <netinet/in.h>
#include <netinet/tcp.h> // For TCP keepalive options
#include <poll.h>        // <-- this defines struct pollfd
#include <string>
#include <sys/select.h>
#include <sys/socket.h>
#include <thread>
#include <vector>

class TcpServer {
  int max_clients = 100;
  int port = 8080;

  int _sockfd;

  struct sockaddr_in _server_addr;

  std::mutex _mu;
  std::vector<int> _clients_fds;

  std::thread *_acceptClientThread = nullptr;
  std::function<void(int clientfd)> newClientHandler;

  std::function<void(int clientfd, std::string)> _recieveFunc;

  void initSocket();
  void initAddress(int port);
  void removeDeadClients();
  std::pair<bool, std::string> acceptClient(int timeout);
  std::pair<bool, std::string> waitForClient(int timeout);

public:
  TcpServer();
  ~TcpServer();

  void initServer(int port, int max_clients);

  void setRecieveFunc(std::function<void(int, std::string)> func);
  void setNewClientHandler(std::function<void(int clientfd)> func);

  // This function sends a message to all connected clients
  // Message is not guaranteed to be received by all clients
  void sendMsgToAllClient(std::string msg);
  std::pair<bool, std::string> sendMsgToClient(int clientfd, std::string msg);

  void startListening();
  void stopListening();

  void _clientConnected(int clientfd);
};
