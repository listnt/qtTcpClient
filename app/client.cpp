#include "pkg/tcp/tcpclient.h"

#include <boost/program_options.hpp>
#include <iostream>
#include <string>

int connectClient(std::string host, int port) {
  TcpClient *client = new TcpClient();

  client->connectTo(host, port);
  client->setRecieveFunc(
      [](std::string msg) { std::cout << "Recieved: " << msg << std::endl; });

  client->startListening();
  client->stopListening();

  return 0;
}
