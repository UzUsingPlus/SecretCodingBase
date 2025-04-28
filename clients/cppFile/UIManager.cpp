#include "UIManager.h"
#include <QObject>
#include <qobject.h>
UIManager::UIManager(Hall* hall,LoginUi* loginui,MainWindow* mainwindow,GameUi* gameui)
    :hall(hall),loginui(loginui),mainwindow(mainwindow),gameui(gameui)
{
    QObject::connect(this->hall,&Hall::loginSlot,this,&UIManager::LoginToHall);
    QObject::connect(this->hall,&Hall::logoutSlot,this,&UIManager::HallToLogin);
    QObject::connect(this->hall,&Hall::enterGameSlot,this,&UIManager::HallToGame);
    QObject::connect(this->hall,&Hall::leaveGameSlot,this,&UIManager::GameToHall);
}
//UIManager::~UIManager(){}
void UIManager::LoginToHall(int code){
    if(code == BHR_SUCC){
        this->loginui->hide();
        this->mainwindow->show();
    }
}
void UIManager::HallToLogin(int code){
    if(code == BHR_SUCC){
        this->mainwindow->hide();
        this->loginui->show();
    }
}
void UIManager::HallToGame(int code){
    if(code == BHR_SUCC){
        this->mainwindow->hide();
        this->gameui->show();
    }
}
void UIManager::GameToHall(int code){
    if(code == BHR_SUCC){
        this->gameui->hide();
        this->mainwindow->show();
    }
}
