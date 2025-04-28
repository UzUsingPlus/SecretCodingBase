#include <QMessageBox>
#include <QString>
#include <string>
#include "loginui.h"
#include "ui_loginui.h"
#include "transPtc.h"
#include "HallInterface.h"
LoginUi::LoginUi(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginUi)
{
    ui->setupUi(this);
    connect(ui->registerButton,&QPushButton::clicked,this,&LoginUi::on_registerButton_clicked);
    connect(ui->loginButton,&QPushButton::clicked,this,&LoginUi::on_loginButton_clicked);
    connect(ui->exitButton,&QPushButton::clicked,this,&LoginUi::on_exitButton_clicked);
    this->show();
}
LoginUi::LoginUi(QWidget *parent,Hall* hall)
    :QWidget(parent)
    ,hall(hall)
    ,ui(new Ui::LoginUi)
{
    ui->setupUi(this);
    connect(ui->registerButton,&QPushButton::clicked,this,&LoginUi::on_registerButton_clicked);
    connect(ui->loginButton,&QPushButton::clicked,this,&LoginUi::on_loginButton_clicked);
    connect(ui->exitButton,&QPushButton::clicked,this,&LoginUi::on_exitButton_clicked);
    QObject::connect(this->hall,&Hall::regisSlot,this,&LoginUi::show_regis_res);
    QObject::connect(this->hall,&Hall::loginSlot,this,&LoginUi::show_login_res);
    this->show();
}
LoginUi::~LoginUi()
{
    delete ui;
}
int LoginUi::waitForRes(int waitTime){
    int res = 0 ;
    time_t initTime ;
    time(&initTime);
    time_t currTime ;
    while( res == 0 ){
        time(&currTime) ;
        if(currTime - initTime >= waitTime)
            break ;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    if( res == 0 )
        return 0 ; //nothing happen
    else if( res < 0 )
        return res  ;
    else return 1;  // something happen
}
void LoginUi::toHall(){
    this->close();
}
void LoginUi::get_user_info(){
    this->hall->fillNamePwd( ui->userNameLineEdit->text().toStdString(),ui->userPwdLineEdit->text().toStdString());
}
void LoginUi::on_registerButton_clicked(){
    this->get_user_info();
    this->hall->buildRegisBhr();
}
void LoginUi::on_loginButton_clicked(){
    this->get_user_info();
    this->hall->buildLoginBhr();
}
void LoginUi::on_exitButton_clicked(){
    this->close();
}
void LoginUi::show_regis_res(int code){
    if(code == BHR_SUCC)
        QMessageBox::information(this,"信息","注册成功！");
    else if(code == NAMEEXIST_ERROR)
        QMessageBox::information(this,"信息","注册失败。用户名已存在");
    else
        QMessageBox::information(this,"信息","未知错误。请稍后重试。");

}
void LoginUi::show_login_res(int code){
    if(code == BHR_SUCC)
        QMessageBox::information(this,"信息","启动！");
    else if(code == FINDUSER_ERROR)
        QMessageBox::information(this,"信息","登录失败。找不到用户名.");
    else if(code == MATCH_ERROR)
        QMessageBox::information(this,"信息","登录失败。用户名与密码不匹配.");
    else
        QMessageBox::information(this,"信息","未知错误。请稍后重试。");

}

