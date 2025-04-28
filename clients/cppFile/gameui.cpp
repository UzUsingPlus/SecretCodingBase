#include "gameui.h"
#include <QMessageBox>
#include <QPainter>
#include "HallInterface.h"
#include <QPushButton>
#include <qpushbutton.h>
#include <conio.h>
#include <QKeyEvent>
GameUi::GameUi(QWidget *parent,Hall* hall)
    : QWidget{parent},hall(hall),ui(new Ui::GameUi)
{
    this->ui->setupUi(this);
    connect(this->ui->begin_game_button,&QPushButton::clicked,this,&GameUi::on_beginGameButton_clicked);
    connect(this->ui->leave_game_button,&QPushButton::clicked,this,&GameUi::on_leaveGameButton_clicked);
    connect(this->hall,&Hall::beginGameSlot,this,&GameUi::show_begin_game_res);
    connect(this->hall,&Hall::leaveGameSlot,this,&GameUi::show_leave_game_res);
    connect(this->hall,&Hall::updateGameSlot,this,&GameUi::show_game_state);
    connect(this->hall,&Hall::endTheGameSlot,this,&GameUi::end_the_game);
    this->ui->begin_game_button->show();
    this->ui->anc_label->hide();
    this->installEventFilter(this);
}
void GameUi::on_beginGameButton_clicked(){
    this->hall->buildBeginGameBhr();
}
void GameUi::on_leaveGameButton_clicked(){
    this->hall->buildLeaveGameBhr();
}
void GameUi::show_begin_game_res(int code){
    if(code == BHR_SUCC){
        this->ui->begin_game_button->hide();
        this->ui->leave_game_button->hide();
    }
    else
        QMessageBox::information(this,"信息","未知错误。请稍后重试。");
}
void GameUi::show_leave_game_res(int code){
    if(code == BHR_SUCC){
        QMessageBox::information(this,"信息","退出房间成功！");
    }
    else
        QMessageBox::information(this,"信息","未知错误。请稍后重试。");
}
void GameUi::show_game_state(int code){
    if(this->hall->judgeGameOver()){
        this->ui->anc_label->hide();
        this->ui->begin_game_button->show();
    }
    //to do : how to show the score ..?
    int score = this->hall->returnTheGameScore() ;
    std::string str = to_string(score) ;
    this->ui->scoreLine->setText(str.c_str());

    update();
}
void GameUi::paintEvent(QPaintEvent* event){
    QWidget::paintEvent(event);
    QPainter painter(this);
    if(!painter.isActive()) return ;

    painter.fillRect(rect(),Qt::white);//clear the screen
    painter.setRenderHint(QPainter::Antialiasing);

    int ballX , ballY , ballRadius ;
    this->hall->calBallPara(ballX,ballY,ballRadius);
    painter.setBrush(Qt::blue);
    painter.drawEllipse(ballX,ballY,ballRadius*2,ballRadius*2);

    int blockX , blockY , blockLength , blockWidth ;
    this->hall->calBlockPara(blockX,blockY,blockLength,blockWidth) ;
    painter.setBrush(Qt::red);
    painter.drawRect(blockX,blockY,blockLength,blockWidth);
}
void GameUi::end_the_game(int code){
    if(code == BHR_SUCC){
        QMessageBox::information(this,"信息","游戏结束！");
        this->ui->begin_game_button->show();
        this->ui->leave_game_button->show();
    }
}
bool GameUi::eventFilter(QObject* obj,QEvent* event){
    if( event->type() == QEvent::KeyPress ){
        QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
        if(keyEvent){
            if(keyEvent && (keyEvent->key() == Qt::Key_A || keyEvent->key() == Qt::Key_Left)){
                this->hall->buildCtrlGameBhr(MOVE_L);
            }else if(keyEvent && (keyEvent->key() == Qt::Key_D || keyEvent->key() == Qt::Key_Right)){
                this->hall->buildCtrlGameBhr(MOVE_R);
            }
        }
    }
    return QObject::eventFilter(obj,event) ;
}

