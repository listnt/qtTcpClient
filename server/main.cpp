#include "tcpserver.h"
#include <QCoreApplication>

#include <QCommandLineParser>
#include <signal.h>

void recieveMsg(std::shared_ptr<TcpServer> server, int clientfd,
                std::string msg);
int launchServer(int port, int max_clients) {
  signal(SIGPIPE, SIG_IGN); // VERY IMPORTANT: IGNORE SIGPIPE

  std::shared_ptr<TcpServer> server = std::make_shared<TcpServer>();
  server->initServer(port, max_clients);
  server->setRecieveFunc([server](int clientfd, std::string msg) {
    recieveMsg(server, clientfd, msg);
  });

  try {
    server->startListening();
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }

  server->stopListening();

  return 0;
}

void recieveMsg(std::shared_ptr<TcpServer> server, int clientfd,
                std::string msg) {
  zap::info << "msg"
            << "Recieved message from client"
            << "data" << msg;

  if (msg == "status") {
    server->_clientConnected(clientfd);
  } else {
    server->sendMsgToClient(clientfd, "Sending back message:\n" + msg);
  }
}

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  QCommandLineParser parser;
  parser.setApplicationDescription("tcp server");
  parser.addHelpOption();
  parser.addVersionOption();

  // A boolean option with a single name (-p)
  QCommandLineOption port_arg(
      "p", QCoreApplication::translate("main", "Tcp port"),
      QCoreApplication::translate("main", "port"), "8080");
  parser.addOption(port_arg);

  // A boolean option with multiple names (-m, --max_clients)
  QCommandLineOption max_clients_arg(
      QStringList() << "m"
                    << "max clients",
      QCoreApplication::translate("main", "Max number of clients."),
      QCoreApplication::translate("main", "max_clients"), "100");
  parser.addOption(max_clients_arg);

  // Process the actual command line arguments given by the user
  parser.process(a);

  const QStringList args = parser.positionalArguments();
  // source is args.at(0), destination is args.at(1)

  bool suckass = false;
  int port = parser.value(port_arg).toInt(&suckass);
  if (!suckass) {
    zap::err << "error" << "port must be a number";
    return 1;
  }

  int max_clients = parser.value(max_clients_arg).toInt(&suckass);
  if (!suckass) {
    zap::err << "error" << "max_clients must be a number";
    return 1;
  }

  launchServer(port, max_clients);

  return 0;
}
