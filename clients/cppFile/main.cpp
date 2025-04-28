#include "mainwindow.h"
#include "winSockTcp.h"
#include "transPtc.h"
#include <QApplication>
#include "UIManager.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    replyQueue* rq = new replyQueue();
    messageQueue* mq = new messageQueue();
    Hall *hall = new Hall(mq);
    winSockTcp wst(rq,mq);
    rq->hall = hall ;
    MainWindow *mainwindow = new MainWindow(nullptr,hall);
    LoginUi *loginui = new LoginUi(nullptr,hall);
    GameUi *gameui = new GameUi(nullptr,hall);
    UIManager *uimanager = new UIManager(hall,loginui,mainwindow,gameui);
    return a.exec();
}
