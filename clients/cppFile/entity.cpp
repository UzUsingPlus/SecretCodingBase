#include "entity.h"
#include <conio.h>
Ball::Ball(float radius,float posX,float posY)
:radius(radius),posX(posX),posY(posY)
{
}
Block::Block(float halLength,float halWidth,float posX,float posY)
:halLength(halLength),halWidth(halWidth),posX(posX),posY(posY)
{
}
BallGameRoom::BallGameRoom()
{
    {
        std::lock_guard<std::mutex> lock(this->updateMutex);
        this->endMark = 1 ;
    }
}
int BallGameRoom::returnTheScore(){
    return this->score ;
}
void BallGameRoom::beginTheGame(){
    {
        std::lock_guard<std::mutex> lock(this->updateMutex);
        this->endMark = 0 ;
    }
}
void BallGameRoom::endTheGame(){

}
void BallGameRoom::upDateRoomInfo(int endMark,int score,Ball& ball,Block& block){
    {
        std::lock_guard<std::mutex> lg(this->updateMutex);
        this->endMark = endMark ;
        this->score = score ;
        this->ball = std::move(ball);
        this->block = std::move(block);
    }
}

Player::Player(){}
Player::Player(int userId, string userName, int userdqCoin, int userStage) {
    this->userId = userId;
    this->userName = userName;
    this->userdqCoin = userdqCoin;
    this->userStage = userStage ;
}
void Player::clearThePlayer() {
    this->userId = -1;
    this->userName = "NULL";
    this->userdqCoin = -1;
    this->userStage = -1;
}
