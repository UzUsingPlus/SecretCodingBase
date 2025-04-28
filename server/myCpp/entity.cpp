#include "entity.h" 
#include "transPtc.h" 
#include <unistd.h>
const int GameFPS = 100 ; 
const int DST = 10 ; //to be test
int BallGameRoom::threadInit(){
    this->calStatusThread = std::thread(&BallGameRoom::calStatus,this);
    return 1 ;        
} 
void Ball::checkMargin(){

}
void BallGameRoom::calStatus(){
    float timeGapInS = 0.01 ;// maybe a global val .. 
    while(!(this->endMark)){
        {
            std::lock_guard<std::mutex> lg(this->dataMutex);
            Ball& ball = this->ball ; 
            ball.Vx = ball.Vx + timeGapInS * ball.Ax ; 
            ball.Vy = ball.Vy + timeGapInS * ball.Ay ; 
            ball.posX = ball.posX + ball.Vx * timeGapInS + timeGapInS*timeGapInS*(ball.Ax)/2 ; 
            ball.posY = ball.posX + ball.Vy * timeGapInS + timeGapInS*timeGapInS*(ball.Ay)/2 ;  
            this->score ++ ; 
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));//10 ms
    }
}
void BallGameRoom::calBlockStatus(int lastUserMove){
    {
        std::lock_guard<std::mutex> lg(this->dataMutex);
        this->block.posX += lastUserMove * DST ; 
    }
}
void BallGameRoom::beginTheGame(){
    this->threadInit();
    {
        std::lock_guard<std::mutex> lg(this->endMarkMutex);
        this->endMark = 0 ; 
    }
}
void BallGameRoom::endTheGame(){
    {
        std::lock_guard<std::mutex> lg(this->endMarkMutex);
        this->endMark = 1 ; 
    }
    this->calStatusThread.join();
}


void BallGameRoom::buildHelper(GameReply* gr){
    {
        std::lock_guard<std::mutex> lg(this->dataMutex);
        gr->endMark = this->endMark ; 
        gr->score = this->score ; 
        gr->ball = this->ball ; 
        gr->block = this->block ; 
    }
}
void Ball::clear(){
    radius = -1 ; 
    posX = -1 ;
    posY = -1 ;
    Vx = -1 ; 
    Vy = -1 ;
    Ax = -1;
    Ay = -1 ;  
}

void Block::clear(){
    halLength = -1 ;
    halWidth = -1 ;
    posX = -1; 
    posY = -1 ; 
}
BallGameRoom::BallGameRoom()
{
    this->endMark = 1 ; 
    this->score = 0 ; 
}
BallGameRoom::BallGameRoom(int userId,int userSocket)
:userId(userId),userSocket(userSocket)
{
    this->endMark = 1 ; 
    this->score = 0 ; 
    //the calculating thread just need to be inited when the game begin !
}


void BallGameRoom::detectCollision(){//to polish
    Ball ball = this->ball;
    if (
		((ball.posY + ball.radius <= block.posY + block.halWidth && ball.posY + ball.radius >= block.posY - block.halWidth)
		||(ball.posY - ball.radius <= block.posY + block.halWidth && ball.posY - ball.radius >= block.posY - block.halWidth))
		&& (ball.posX <= block.posX + block.halLength && ball.posX >= block.posX - block.halLength)
		)
	{
		if (ball.Vy < 590) {
		}
		else {
			ball.Vy = -ball.Vy;
		}
		// if (stageNum != 1) {
		// 	srand((unsigned)time(NULL));
		// 	float Vrd = (float)(rand() % 300);
		// 	if (trace[0] > trace[1]) {
		// 		ball.Vx = ball.Vx + Vrd;
		// 	}
		// 	else if (trace[0] < trace[1]) {
		// 		ball.Vx = ball.Vx - Vrd;
		// 	}
		// }
	}
}
void BallGameRoom::clear(){
    this->userId = -1 ; 
    this->userSocket = -1;
    this->score = 0 ;
    this->ball.clear();
    this->block.clear();
}
Player::Player(){}
Player::Player(int userId, string userName, int userdqCoin, Player* nextPlayer) {
    this->userId = userId;
    this->userName = userName;
    this->userdqCoin = userdqCoin;
    this->nextPlayer = nextPlayer;
}
void Player::clearThePlayer() {
    this->userId = -1;
    this->userName = "NULL";
    this->userPwd = "NULL";
    this->userdqCoin = -1;
    this->userState = -1;
    this->readyMark = -1 ; 
    this->roomId = -1 ; 
    this->roomPos = -1 ; 
    this->cliSock = -1; 
    this->nextPlayer = nullptr ; 
}