
#include "tcpclient.h"
#include <iostream>
#include <sys/socket.h>

TcpClient::TcpClient() {}

TcpClient::~TcpClient() {
    this->_isconnected = false;
    this->stopListening();

    close(this->_sockfd);
}

void TcpClient::connectTo(std::string host, int port) {
    this->initSocket();
    this->initConnection(host, port);
}

void TcpClient::setRecieveFunc(std::function<void(std::string)> func) {
    if (this->_recieveThread != nullptr) {
        throw std::runtime_error("Recieve thread already started");
    }

    this->_recieveFunc = func;
}

void TcpClient::initSocket() {
    if (this->_sockfd!=0){
        close(this->_sockfd);
    }

    this->_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_sockfd == -1) {
        throw std::runtime_error("Socket creation failed");
    }
}

void TcpClient::initConnection(std::string host, int port) {
    struct in_addr addr;
    int ipv4Success = inet_pton(AF_INET,host.c_str(),&addr);
    if (ipv4Success==0){
        throw std::runtime_error("Invalid IP address");
    }

    int inetSuccess = inet_aton(host.c_str(), &_serv_addr.sin_addr);
    if (!inetSuccess) {
        throw std::runtime_error("Invalid IP address");
    }

    _serv_addr.sin_family = AF_INET;
    _serv_addr.sin_port = htons(port);
    int connectionResult = connect(this->_sockfd, (struct sockaddr *)&_serv_addr,
                                   sizeof(_serv_addr));

    if (connectionResult == -1) {
        throw std::runtime_error("Connection failed");
    }

    this->_isconnected = true;
}

std::pair<bool, std::string> TcpClient::sendMsg(std::string msg) {
    if (this->_isconnected == false) {
        return {false, "Not connected"};
    }

    int res = send(this->_sockfd, msg.c_str(), msg.length(), 0);
    if (res == -1) {
        this->_isconnected = false;
        return {false, "Send failed: " + std::string(strerror(errno))};
    }

    if (res != msg.length()) {
        return {false, "Send failed"};
    }

    return {true, ""};
}

void TcpClient::startListening() {
    this->_recieveThread = new std::thread([this]() {
        std::cout << "Listening for messages" << std::endl;

        char buffer[1024];
        while (true) {
            if (this->_isconnected == false) {
                break;
            }

            struct pollfd pfd;
            pfd.fd = this->_sockfd;
            pfd.events = POLLIN;

            int res = poll(&pfd, 1, 1);
            if (res == -1) {
                this->_isconnected = false;
                break;
            }

            if (res == 0) {
                continue;
            }

            int bytesReceived = recv(this->_sockfd, buffer, 1024, 0);
            if (bytesReceived == -1) {
                this->_isconnected = false;
                std::cout << "Error: " << strerror(errno) << std::endl;
                break;
            }
            if (bytesReceived == 0) {
                std::cout << "Server disconnected" << std::endl;
                this->_isconnected = false;
                break;
            }

            buffer[bytesReceived] = '\0';
            std::string message(buffer);

            this->_recieveFunc(message);
        }
    });
}

void TcpClient::stopListening() {
    this->_isconnected = false;

    if (this->_recieveThread != nullptr) {
        this->_recieveThread->join();
        delete this->_recieveThread;
        this->_recieveThread = nullptr;
    }
}

bool TcpClient::isConnected(){
    return this->_isconnected;
}
