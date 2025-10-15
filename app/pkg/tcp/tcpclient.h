
#include <arpa/inet.h>
#include <cstdio>
#include <cstring> // For strerror
#include <functional>
#include <netinet/in.h>
#include <poll.h> // <-- this defines struct pollfd
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

class TcpClient {
  int _sockfd;
  struct sockaddr_in _serv_addr;
  std::string _ip = "";
  std::thread *_recieveThread = nullptr;
  std::function<void(std::string)> _recieveFunc = nullptr;
  bool _isconnected = false;

public:
  TcpClient();
  ~TcpClient();

  void initSocket();
  void initConnection(std::string host, int port);

  void connectTo(std::string host, int port);

  std::pair<bool, std::string> sendMsg(std::string msg);
  void setRecieveFunc(std::function<void(std::string)> func);
  bool isConnected();


  void startListening();
  void stopListening();
};
