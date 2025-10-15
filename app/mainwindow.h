#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "pkg/tcp/tcpclient.h"
#include <thread>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    TcpClient *client = nullptr;
    std::thread *statuschecker = nullptr;

    QPixmap greenCircle = QPixmap(":/images/greencircle.png");
    QPixmap redCircle = QPixmap(":/images/circle-16.png");

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void init();
    void statusCheck();
    void connectTo(std::string host,int port);
    std::pair<bool,std::string> sendMessage(std::string msg);

    void processServerResponse(std::string msg);



private:
    Ui::MainWindow *ui;
private slots:
    void onConnectPress();
    void onSendMsgPress();
};
#endif // MAINWINDOW_H
