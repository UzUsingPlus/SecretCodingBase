#include "entity.h" 
#include "transPtc.h" 
#include <unistd.h>
const int GameFPS = 100 ; 
const int DST = 20 ; //to be test
int BallGameRoom::threadInit(){
    this->calStatusThread = std::thread(&BallGameRoom::calStatus,this);
    return 1 ;        
} 
Ball::Ball(){
    //640 x 480 
    this->radius = 5 ; 
    this->posX = 320 ;
    this->posY = 25 ;
    this->Vx = 0 ; 
    this->Vy = 0 ; 
    this->Ax = 0 ; 
    this->Ay = 10 ;  
}
Block::Block(){
    this->halLength = 20; 
    this->halWidth = 5 ; 
    this->posX = 300 ; 
    this->posY = 470 ; 
}
void BallGameRoom::calStatus(){
    float timeGapInS = 0.001 ;// maybe a global val .. 
    while(!(this->endMark)){
        {
            std::lock_guard<std::mutex> lg(this->dataMutex); 
            ball.Vx = ball.Vx + timeGapInS * ball.Ax ; 
            ball.Vy = ball.Vy + timeGapInS * ball.Ay ; 
            ball.posX = ball.posX + ball.Vx * timeGapInS + timeGapInS*timeGapInS*(ball.Ax)/2 ; 
            ball.posY = ball.posY + ball.Vy * timeGapInS + timeGapInS*timeGapInS*(ball.Ay)/2 ;  
            this->score ++ ; 
            this->detectCollision();
            this->detectMargin();
            if(this->endJudge()){
                this->endMark = 1 ;    
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));//10 ms
    }
}
void BallGameRoom::ctrlBlock(int lastUserMove){
    {
        std::lock_guard<std::mutex> lg(this->dataMutex);
        this->block.posX += lastUserMove * DST ; 
    }
}
void BallGameRoom::beginTheGame(){
    {
        std::lock_guard<std::mutex> lg(this->endMarkMutex);
        this->endMark = 0 ; 
    }
    cout << 0 << endl ; 
    this->remake();
    cout << 1 << endl ; 
    this->threadInit();
    cout << 2 << endl ; 
}
void BallGameRoom::endTheGame(){
    {
        std::lock_guard<std::mutex> lg(this->endMarkMutex);
        this->endMark = 1 ; 
    }
    if(this->calStatusThread.joinable())
        this->calStatusThread.join();
}


void BallGameRoom::buildHelper(GameReply* gr){
    {
        std::lock_guard<std::mutex> lg(this->dataMutex);
        gr->userId = this->userId ; 
        gr->userSocket = this->userSocket ; 
        gr->endMark = this->endMark ; 
        gr->score = this->score ; 
        gr->ball = this->ball ; 
        gr->block = this->block ; 
    }
}
void Ball::clear(){
    this->radius = 5 ; 
    this->posX = 320 ;
    this->posY = 25 ;
    this->Vx = 0 ; 
    this->Vy = 0 ; 
    this->Ax = 0 ; 
    this->Ay = 10 ;  
}
void Block::clear(){
    this->halLength = 20; 
    this->halWidth = 5 ; 
    this->posX = 300 ; 
    this->posY = 470 ; 
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
int BallGameRoom::returnEndMark(){
    return this->endMark ; 
}
int BallGameRoom::endJudge(){
    return this->ball.posY - this->ball.radius > 480 ;  
}
void BallGameRoom::detectCollision(){//to polish
    Ball ball = this->ball;
    if (
		(( block.posY <= ball.posY + ball.radius && ball.posY + ball.radius <= block.posY + 2*block.halWidth )
		||( block.posY <= ball.posY - ball.radius && ball.posY - ball.radius <= block.posY + 2*block.halWidth ))
		&& ( block.posX <= ball.posX && ball.posX <= block.posX + 2*block.halLength )
		)
	{
        this->ball.Vy = -this->ball.Vy ; 
        srand((unsigned)time(NULL));
		float Vrd = (float)(rand() % 100) - 50 ;
        this->ball.Vx += Vrd ; 
		// if (ball.Vy < 590) {
		// }
		// else {
		// 	ball.Vy = -ball.Vy;
		// }
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
void BallGameRoom::detectMargin(){
    if(this->ball.posX <= 0 && this->ball.Vx < 0 ){
        this->ball.posX += 680 ; 
    }else if(this->ball.posX >= 680 && this->ball.Vx > 0){
        this->ball.posX -= 680 ; 
    }
}
void BallGameRoom::clear(){
    this->userId = -1 ; 
    this->userSocket = -1;
    this->score = 0 ;
    this->ball.clear();
    this->block.clear();
}
void BallGameRoom::remake(){
    {
        std::lock_guard<std::mutex> lock(this->dataMutex); 
        this->score = 0 ; 
        this->ball.clear();
        this->block.clear();
    }
}
Player::Player(int userId, string userName, int userdqCoin,int cliSock,Player* nextPlayer) 
:userId(userId),userName(userName),userdqCoin(userdqCoin),cliSock(cliSock),nextPlayer(nextPlayer)
{
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
PlayerInfo::PlayerInfo(int userId,std::string userName,int userdqCoin,int userStage)
:userId(userId),userName(userName),userdqCoin(userdqCoin),userStage(userStage)
{

}
RankElement::RankElement(int userId,std::string userName,int score)
:userId(userId),userName(userName),score(score)
{

}
BulletinElement::BulletinElement(int userId,std::string userName,std::string words)
:userId(userId),userName(userName),words(words)
{

}