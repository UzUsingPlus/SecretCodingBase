#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "transPtc.h"

#include <QInputDialog>
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->logout_button,&QPushButton::clicked,this,&MainWindow::on_logout_button_clicked);
    connect(ui->enter_game_button,&QPushButton::clicked,this,&MainWindow::on_enter_game_button_clicked);
    connect(ui->leave_words_button,&QPushButton::clicked,this,&MainWindow::on_leave_words_button_clicked);
    QObject::connect(this->hall,&Hall::logoutSlot,this,&MainWindow::show_logout_res);
    QObject::connect(this->hall,&Hall::leaveWordsSlot,this,&MainWindow::show_leave_words_res);
    QObject::connect(this->hall,&Hall::enterGameSlot,this,&MainWindow::show_enter_game_res);
    QObject::connect(this->hall,&Hall::updateHallSlot,this,&MainWindow::show_hall_state);
    this->hide();
}
MainWindow::MainWindow(QWidget* parent,Hall*hall)
    :QMainWindow(parent)
    ,hall(hall)
    ,ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->logout_button,&QPushButton::clicked,this,&MainWindow::on_logout_button_clicked);
    connect(ui->enter_game_button,&QPushButton::clicked,this,&MainWindow::on_enter_game_button_clicked);
    connect(ui->leave_words_button,&QPushButton::clicked,this,&MainWindow::on_leave_words_button_clicked);
    QObject::connect(this->hall,&Hall::logoutSlot,this,&MainWindow::show_logout_res);
    QObject::connect(this->hall,&Hall::leaveWordsSlot,this,&MainWindow::show_leave_words_res);
    QObject::connect(this->hall,&Hall::enterGameSlot,this,&MainWindow::show_enter_game_res);
    QObject::connect(this->hall,&Hall::updateHallSlot,this,&MainWindow::show_hall_state);
    this->hide();
}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_logout_button_clicked(){
    this->hall->buildLogoutBhr();
}
void MainWindow::on_enter_game_button_clicked(){
    this->hall->buildEnterGameBhr();
}
void MainWindow::on_leave_words_button_clicked(){
    QString userText = QInputDialog::getText(nullptr,"留言","请输入你的留言:");
    this->hall->buildLeaveWordsBhr(userText.toStdString());
    //recv the words
}
void MainWindow::show_logout_res(int code){
    if(code == BHR_SUCC){
        QMessageBox::information(this,"信息","退出登录成功！");
    }else{
        QMessageBox::information(this,"信息","未知错误");
    }
}
void MainWindow::show_enter_game_res(int code){
    if(code == BHR_SUCC){
        QMessageBox::information(this,"信息","进入房间成功！");
    }else{
        QMessageBox::information(this,"信息","未知错误");
    }
}
void MainWindow::show_leave_words_res(int code){
    if(code == BHR_SUCC){
        QMessageBox::information(this,"信息","留言成功！");
    }else{
        QMessageBox::information(this,"信息","未知错误");
    }
}
void MainWindow::show_hall_state(int code){
    if(code == BHR_SUCC){// nonono just test!!
        this->show_rank();
        this->show_words();
        this->show_playerList();
    }
}
void MainWindow::show_rank(){
    vector<QString> qs = this->hall->rankToQString();
    int n = qs.size() ;
    switch (n){
    case 1 :{
        ui->rank1->setText(qs[0]);
        break ;
    }case 2:{
        ui->rank1->setText(qs[0]);
        ui->rank2->setText(qs[1]);
        break ;
    }case 3:{
        ui->rank1->setText(qs[0]);
        ui->rank2->setText(qs[1]);
        ui->rank3->setText(qs[2]);
        break ;
    }default:{
        break ;
    }
    }
}
void MainWindow::show_words(){
    ui->words->setText(this->hall->wordsToQString());
}
void MainWindow::show_playerList(){
    //to do
}
