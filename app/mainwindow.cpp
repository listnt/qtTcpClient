#include "./ui_mainwindow.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QPushButton>

MainWindow::MainWindow(
    QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(onConnectPress()));
  connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(onSendMsgPress()));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::init() {
  this->statuschecker = new std::thread([this]() {
    while (true) {
      this->statusCheck();
      std::this_thread::sleep_for(std::chrono::seconds(5));
    }
  });

  this->client = new TcpClient();
  this->client->setRecieveFunc(
      [this](std::string msg) { this->processServerResponse(msg); });
}

void MainWindow::statusCheck() {
  if (!this->client) {
    return;
  }

  this->client->sendMsg("status");
  if (!this->client->isConnected()) {
    this->ui->label_2->setPixmap(this->redCircle);
    ui->label_3->clear();
  } else {
    this->ui->label_2->setPixmap(this->greenCircle);
  }
}

void MainWindow::connectTo(
    std::string host, int port) {
  if (this->client->isConnected()) {
    this->client->stopListening();
  }

  try {
    this->client->initSocket();
    this->client->initConnection(host, port);
  } catch (const std::exception &e) {
    QMessageBox::warning(nullptr, "Warning!", e.what());
    return;
  } catch (...) {
    zap::err << "err" << "unknown exception";
    return;
  }

  this->client->startListening();
  if (this->client->isConnected()) {
    this->ui->label_2->setPixmap(this->greenCircle);
  }
}

std::pair<bool, std::string> MainWindow::sendMessage(
    std::string msg) {
  if (this->client->isConnected()) {
    return this->client->sendMsg(msg);
  }

  return {false, "Client not connected"};
}

void MainWindow::processServerResponse(
    std::string msg) {
  if (msg.find("currentClients") != std::string::npos) {
    int currentClients = 0;
    int clientfd;
    sscanf(msg.c_str(), "status=connected; clientfd=%d; currentClients=%d;",
           &clientfd, &currentClients);

    QString text = "currently connected: " + QString::number(currentClients);

    this->ui->label_3->setText(text);
  } else {
    QString text = QString::fromStdString(msg);
    this->ui->listWidget->addItem(text);
  }
}

void MainWindow::onConnectPress() {
  QString input = ui->lineEdit->text();

  auto list = input.split(':');
  if (list.size() != 2) {
    QMessageBox::
        warning(nullptr, "Warning!", "incorrect format, requires host:port");
    return;
  }

  int ip = std::stoi(list[1].toStdString());

  this->connectTo(list[0].toStdString(), ip);
  this->ui->listWidget->clear();
}

void MainWindow::onSendMsgPress() {
  if (!this->client->isConnected()) {
    QMessageBox::warning(nullptr, "Warning!", "Client is not connected");
    return;
  }

  QString text = this->ui->textEdit->toPlainText();

  this->client->sendMsg(text.toStdString());

  return;
}
