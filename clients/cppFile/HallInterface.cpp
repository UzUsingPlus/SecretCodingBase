#include "HallInterface.h"
#include "transPtc.h"
#include "gameui.h"
#include "loginui.h"
#include <conio.h>
#include <iostream>
#include <QKeyEvent>
#include <QApplication>
const int MOVE_L = -1 ;
const int MOVE_R = 1 ;
Hall::Hall(){
    this->localUserId = -1 ;
    this->localUserName = "";
    this->localUserPwd = "";
    this->localUserSocket = -1;
    this->localUserDqCoin = -1 ;
    this->localRoomId = -1 ;
    //BallGameRoom localRoom
    this->maxGameRoomId = -1 ;
    this->onlineUserNum = -1 ;
    this->onlineRoomNum = -1;
    this->currWordsIdx = 0 ; //vital
    this->playerList.clear();
    this->rank.clear();
    //this->bulletin.clear();
    this->updateMark = 0 ;
    this->endMark = 0 ;
    this->mq = nullptr;
    this->gameEndMark = 1 ;
    //this->installEventFilter(this);
    //qApp->installEventFilter(this);
    // this->loginui = new LoginUi(nullptr,this);
    // this->loginui->show();
    // this->gameui = new GameUi(nullptr,this);
}
Hall::Hall(messageQueue* mq){
    this->localUserId = -1 ;
    this->localUserName = "";
    this->localUserPwd = "";
    this->localUserSocket = -1;
    this->localUserDqCoin = -1 ;
    this->localRoomId = -1 ;
    //BallGameRoom localRoom
    this->maxGameRoomId = -1 ;
    this->onlineUserNum = -1 ;
    this->onlineRoomNum = -1;
    this->currWordsIdx = 0 ; //vital
    this->playerList.clear();
    this->rank.clear();
    //this->bulletin.clear();
    this->updateMark = 0 ;
    this->endMark = 0 ;
    this->mq = mq;//!!!!!
    this->gameEndMark = 1 ;
    //this->installEventFilter(this);
    //qApp->installEventFilter(this);
}
void Hall::fillNamePwd(string userName,string Pwd){
    this->localUserName = userName ;
    this->localUserPwd = Pwd;
}
void Hall::Pushmq(UserBhr* userBhr){
    this->mq->pushUserBhr(userBhr);
}
QString Hall::wordsToQString(){
    string str = "No:" + to_string(this->currBle.userId) + " "+ this->currBle.userName + " " + this->currBle.words;
    return QString(str.c_str());
}
vector<QString> Hall::rankToQString(){
    vector<QString> res;
    auto rankEleToQString = [&](string str,int userId,string userName,int score)->QString{
        str.append(to_string(userId)+" "+userName+" "+to_string(score));
        QString qstr(str.c_str());
        return qstr ;
    };
    vector<string> vec{"Gold Medalist: No.","Sliver Medalist: No.","Bronze Medalist: No."};
    for(int i = 0 ; i < this->rank.size() ; i++){
        res.emplace_back(rankEleToQString(vec[i],this->rank[i].userId,this->rank[i].userName,this->rank[i].score));
    }
    return res ;
}
void Hall::regisReply(int code){
    this->regisSlot(code);
}
void Hall::loginReply(int code,int userId,int dqCoin,int userSocket){
    if( code == BHR_SUCC){
        this->localUserId = userId ;
        this->localUserDqCoin = dqCoin ;
        this->localUserSocket = userSocket ;
    }
    this->loginSlot(code);
}
void Hall::logoutReply(int code){
    this->logoutSlot(code);
}
void Hall::leaveWordsReply(int code){
    this->leaveWordsSlot(code);    
}
void Hall::enterGameReply(int code,int roomId){
    if(code == BHR_SUCC)
        this->localRoomId = roomId ;
    this->enterGameSlot(code);
}
void Hall::beginGameReply(int code){
    if(code == BHR_SUCC){
        {
            std::lock_guard<std::mutex> lock(this->gameEndMarkMutex);
            this->gameEndMark = 0 ;
        }
        //this->GameListenKB = std::thread(&Hall::listenKB,this);
        this->localRoom.beginTheGame();//to do yet nothing in the func
    }
    this->beginGameSlot(code);
}
void Hall::leaveGameReply(int code){
    if(code == BHR_SUCC)
        this->localRoomId = -1 ;
    this->leaveGameSlot(code);
}
void Hall::updateHallInfo(int code,vector<Player>&playerList,BulletinElement &currWords,vector<RankElement>&rank){
    if(code == BHR_SUCC){
        {
            std::lock_guard<std::mutex> lg(this->updateMarkMutex);
            this->playerList = std::move(playerList);
            this->currBle = std::move(currWords);
            this->rank = std::move(rank);
            this->updateHallSlot(code);
        }
    }
}
void Hall::updateGameInfo(int code,int endMark,int score,Ball ball,Block block){
    if(code == BHR_SUCC){
        if( endMark == 1 ){
            {
                std::lock_guard<std::mutex> lock(this->gameEndMarkMutex);
                this->gameEndMark = 1 ;
            }
            if(this->GameListenKB.joinable())
                this->GameListenKB.join();
            this->localRoom.endTheGame();
            this->endTheGameSlot(code);
        }
        this->localRoom.upDateRoomInfo(endMark,score,ball,block);
        this->updateGameSlot(code);
    }
}
int Hall::returnTheGameScore(){
    return this->localRoom.score ;
}
void Hall::listenKB(){
    while(!(this->gameEndMark)){
        if(_kbhit()){
            char c = _getch() ;
            if( c == 'a' ){
                GameBhr* gameBhr = new GameBhr() ;
                gameBhr->fillBhr(this->localUserId,this->localUserSocket,this->localUserSocket);
                gameBhr->buildGameBhr(this->localRoomId,MOVE_L);
                this->mq->pushUserBhr(gameBhr);
            }
            else if(c =='d'){
                GameBhr* gameBhr = new GameBhr() ;
                gameBhr->fillBhr(this->localUserId,this->localUserSocket,this->localUserSocket);
                gameBhr->buildGameBhr(this->localRoomId,MOVE_R);
                this->mq->pushUserBhr(gameBhr);
            }else if( c == 0 || c==-32){
                c = _getch();
                if( c == 75 ){
                    GameBhr* gameBhr = new GameBhr() ;
                    gameBhr->fillBhr(this->localUserId,this->localUserSocket,this->localUserSocket);
                    gameBhr->buildGameBhr(this->localRoomId,MOVE_L);
                    this->mq->pushUserBhr(gameBhr);
                }else if( c == 77 ){
                    GameBhr* gameBhr = new GameBhr() ;
                    gameBhr->fillBhr(this->localUserId,this->localUserSocket,this->localUserSocket);
                    gameBhr->buildGameBhr(this->localRoomId,MOVE_R);
                    this->mq->pushUserBhr(gameBhr);
                }
            }
        }

    }
}

RegisBhr* Hall::buildRegisBhr(){
    RegisBhr* regisBhr = new RegisBhr();
    regisBhr->fillBhr(this->localUserId,this->localUserSocket,REGIS_BHR);
    regisBhr->buildRegisBhr(this->localUserName,this->localUserName);
    this->mq->pushUserBhr(regisBhr);
    return regisBhr ;
}
RegisBhr* Hall::buildLoginBhr(){
    RegisBhr* regisBhr = new RegisBhr();
    regisBhr->fillBhr(this->localUserId,this->localUserSocket,LOGIN_BHR);
    regisBhr->buildRegisBhr(this->localUserName,this->localUserPwd);
    this->mq->pushUserBhr(regisBhr);
    return regisBhr ;
}
HallBhr* Hall::buildLogoutBhr(){
    HallBhr* hallBhr = new HallBhr();
    hallBhr->fillBhr(this->localUserId,this->localUserSocket,LOGOUT_BHR);
    hallBhr->buildHallBhr("");
    this->mq->pushUserBhr(hallBhr);
    return hallBhr ;
}
HallBhr* Hall::buildEnterGameBhr(){
    HallBhr* hallBhr = new HallBhr();
    hallBhr->fillBhr(this->localUserId,this->localUserSocket,ENTER_GAME_BHR);
    hallBhr->buildHallBhr("");
    this->mq->pushUserBhr(hallBhr);
    return hallBhr ;
}
HallBhr* Hall::buildLeaveWordsBhr(string words){
    HallBhr* hallBhr = new HallBhr();
    hallBhr->fillBhr(this->localUserId,this->localUserSocket,LEAVE_WORDS_BHR);
    hallBhr->buildHallBhr(words);
    this->mq->pushUserBhr(hallBhr);
    return hallBhr ;
}
GameBhr* Hall::buildBeginGameBhr(){
    GameBhr* gameBhr = new GameBhr();
    gameBhr->fillBhr(this->localRoomId,this->localUserSocket,BEGIN_GAME_BHR);
    gameBhr->buildGameBhr(this->localRoomId,-1);
    this->mq->pushUserBhr(gameBhr);
    return gameBhr ;
}
GameBhr* Hall::buildCtrlGameBhr(int moveDir){
    GameBhr* gameBhr = new GameBhr();
    gameBhr->fillBhr(this->localRoomId,this->localUserSocket,CTRL_GAME_BHR);
    gameBhr->buildGameBhr(this->localRoomId,moveDir);
    this->mq->pushUserBhr(gameBhr);
    return gameBhr ;
}
GameBhr* Hall::buildLeaveGameBhr(){
    GameBhr* gameBhr = new GameBhr();
    gameBhr->fillBhr(this->localRoomId,this->localUserSocket,LEAVE_GAME_BHR);
    gameBhr->buildGameBhr(this->localRoomId,-1);
    this->mq->pushUserBhr(gameBhr);
    return gameBhr ;
}
void Hall::calBlockPara(int &x,int &y,int &length,int &width){
    x = this->localRoom.block.posX ;
    y = this->localRoom.block.posY ;
    length = this->localRoom.block.halLength*2 ;
    width = this->localRoom.block.halWidth*2;
}
void Hall::calBallPara(int &x,int &y,int &radius){
    x = this->localRoom.ball.posX ;
    y = this->localRoom.ball.posY ;
    radius = this->localRoom.ball.radius ;
}
int Hall::judgeGameOver(){
    return this->localRoom.endMark ;
}




