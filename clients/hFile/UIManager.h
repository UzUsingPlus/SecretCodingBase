#ifndef UIMANAGER_H
#define UIMANAGER_H
#include "loginui.h"
#include "mainwindow.h"
#include "gameui.h"
class UIManager :public  QObject{
    Q_OBJECT
private:
    Hall* hall ;
    LoginUi* loginui ;
    MainWindow* mainwindow ;
    GameUi* gameui;
public :
    explicit UIManager(Hall* hall,LoginUi* loginui = nullptr,MainWindow* mainwindow = nullptr,GameUi* gameui=nullptr);
    //UIManager::~UIManager();
public slots:
    void LoginToHall(int code);
    void HallToLogin(int code);
    void HallToGame(int code);
    void GameToHall(int code);
};
#endif // UIMANAGER_H
