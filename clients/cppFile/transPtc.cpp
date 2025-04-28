#include "transPtc.h"
#include "HallInterface.h"
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <iostream>
using namespace std ;
//userStage
const int REGIS_BHR = 10 ; 
const int LOGIN_BHR = 20 ;
const int LOGOUT_BHR = 110 ;
const int LEAVE_WORDS_BHR = 120 ;
const int ENTER_GAME_BHR = 130;
const int UPDATE_HALL_INFO_BHR = 300 ;
const int BEGIN_GAME_BHR = 210;
const int LEAVE_GAME_BHR = 220 ;
const int UPDATE_GAME_INFO_BHR = 230 ;
const int CTRL_GAME_BHR = 240 ;
//code
const int BHR_SUCC = 100 ;
const int NAMEEXIST_ERROR = 400 ;
const int FINDUSER_ERROR = 410 ;
const int MATCH_ERROR = 420 ;
const int EXIT_ERROR = 430 ;
const int WORD_ERROR = 440 ;
const int ROOMID_ERROR = 450 ;
const int UNKNOWN_ERROR = 666 ;
const int GAME_OVER = 999 ;
using namespace std ;
//serReply content :

void from_json(const json &js , Player& p){
    js.at("userId").get_to(p.userId);
    js.at("userName").get_to(p.userName);
    js.at("userStage").get_to(p.userStage);
    js.at("userdqCoin").get_to(p.userdqCoin);
}
void from_json(const json &js,RankElement& re){
    js.at("userId").get_to(re.userId);
    js.at("userName").get_to(re.userName);
    js.at("score").get_to(re.score);
}
void from_json(const json &js,BulletinElement& be){
    js.at("userId").get_to(be.userId);
    js.at("userName").get_to(be.userName);
    js.at("words").get_to(be.words);
}
void from_json(const json &js,HallBroadCastReply& hr){
    js.at("playerList").get_to(hr.playerList);
    js.at("rank").get_to(hr.rank);
    js.at("currBle").get_to(hr.currBle);
    //js.at("bulletin").get_to(hr.bulletin);
}
void SerReply::fillBasicInfo(json js){
    this->code = js["code"];
    this->userId = js["userId"];
    this->userSocket = js["userSocket"];
    this->userStage = js["userStage"];
}

void RegisReply::dealReply(HallInterface* hall){
    switch(this->userStage){//to do
        case REGIS_BHR:{
            hall->regisReply(this->code);
            break ;
        }
        case LOGIN_BHR :{
            hall->loginReply(this->code,this->userId,this->dqCoin,this->userSocket);
            break ;
        }default:{
            //nothing to do
            break ;
        }
    }
}
void HallReply::dealReply(HallInterface* hall){
    switch(this->userStage){
        case LOGOUT_BHR:{
            hall->logoutReply(this->code);
            break ;
        }case LEAVE_WORDS_BHR:{
            hall->leaveWordsReply(this->code);
            break ;
        }case ENTER_GAME_BHR:{
            hall->enterGameReply(this->code,this->roomId);
            break ;
        }
        default:{
            break ;
        }
    }
}
void GameReply::dealReply(HallInterface* hall){
    switch(this->userStage){
    case BEGIN_GAME_BHR:{
        hall->beginGameReply(this->code);
        break ;
    }case LEAVE_GAME_BHR:{
        hall->leaveGameReply(this->code);
        break ;
    }case UPDATE_GAME_INFO_BHR:{
        hall->updateGameInfo(this->code,this->endMark,this->score,this->ball,this->block);
        break ;
    }
    default:{
        break ;
    }
    }
}
void HallBroadCastReply::dealReply(HallInterface* hall){
    if( this->userStage == UPDATE_HALL_INFO_BHR ){
        hall->updateHallInfo(this->code,this->playerList,this->currBle,this->rank);
    }
}

void RegisReply::analyzeJsonToStruct(json js){
    this->fillBasicInfo(js) ;
    this->dqCoin = js["dqCoin"] ;
}
void HallReply::analyzeJsonToStruct(json js){
    this->fillBasicInfo(js);
    this->roomId = js["roomId"];
}
void GameReply::analyzeJsonToStruct(json js){
    this->fillBasicInfo(js);
    this->endMark = js["endMark"];
    this->score = js["score"];
    Ball ball{
            js["ball"]["radius"],
            js["ball"]["posX"],
            js["ball"]["posY"]
    };
    Block block{
        js["block"]["halLength"],
        js["block"]["halWidth"],
        js["block"]["posX"],
        js["block"]["posY"],
    };
    this->ball = ball;
    this->block = block;
}

void HallBroadCastReply::analyzeJsonToStruct(json js){
    this->fillBasicInfo(js);
    from_json(js,*this);
}
//usrBhr content:
UserBhr::UserBhr(int code,int userId,int userSocket,int userStage,string addition)
    :code(code),userId(userId),userSocket(userSocket),userStage(userStage),addtion(addtion)
{

}
int UserBhr::fillBhr(int userId,int userSocket,int userStage){
    this->userId = userId ;
    this->userSocket = userSocket ;
    this->userStage = userStage ;
    return 1 ;
}
void RegisBhr::buildRegisBhr(string userName,string userPwd){
    this->userName = userName ;
    this->userPwd = userPwd ;
}
void HallBhr::buildHallBhr(string words){
    this->words = words ;
}
void GameBhr::buildGameBhr(int roomId,int moveDir){
    this->roomId = roomId ;
    this->moveDir = moveDir ;
}
json RegisBhr::struct_to_json(){
    json js = json{
        {"code",this->code},
        {"userId",this->userId},
        {"userSocket",this->userSocket},
        {"userStage",this->userStage},
        {"userName",this->userName},
        {"userPwd",this->userPwd}
    };
    return js ;
}
json HallBhr::struct_to_json(){
    json js = json{
        {"code",this->code},
        {"userId",this->userId},
        {"userSocket",this->userSocket},
        {"userStage",this->userStage},
        {"words",this->words}
    };
    return js ;
}
json GameBhr::struct_to_json(){
    cout << " ready to json " <<endl ;
    json js = json{
        {"code",this->code},
        {"userId",this->userId},
        {"userSocket",this->userSocket},
        {"userStage",this->userStage},
        {"roomId",this->roomId},
        {"moveDir",this->moveDir}
    };
    cout << "to json " <<endl ;
    return js ;
}
//
//
// and beneath content is about replyQueue and messageQueue
replyQueue::replyQueue(){
    {
        std::lock_guard<std::mutex> lg(this->endMarkMux);
        this->endMark = 0 ;
    }
    this->threadInit();
}
replyQueue::~replyQueue(){
    {
        std::lock_guard<std::mutex> lg(this->endMarkMux);
        this->endMark = 1 ;
    }
    {
        std::lock_guard<std::mutex> lg(this->mux);
        while(this->queue.size()){
            this->queue.pop();
        }
    }
    this->dealThread.join();
}
int replyQueue::pushSerReply(SerReply* serReply){
    {
        std::lock_guard<std::mutex> lg(this->mux);
        this->queue.push(serReply);
    }
    return 1;  
}
int replyQueue::threadInit(){
    this->dealThread = std::thread(&replyQueue::dealConstantly,this);
    return 1 ;
}
int replyQueue::dealConstantly(){
    while(!(this->endMark)){
        {
            if( this->hall != nullptr ){
                SerReply* serReply = this->popSerReply();
                if( serReply ){
                    serReply->dealReply(this->hall);
                    delete serReply;
                }
                else
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            else
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
    return 1 ;
}
SerReply* replyQueue::popSerReply(){
    SerReply* ret =  nullptr ; 
    {
        std::lock_guard<std::mutex> lg(this->mux);
        if(this->queue.size()){
            ret = this->queue.front();
            this->queue.pop();
        }
    }
    return ret ; 
}
messageQueue::messageQueue(){
    {
        std::lock_guard<std::mutex> lg(this->endMarkMux);
        this->endMark = 0 ;
    }

}
messageQueue::~messageQueue(){
    {
        std::lock_guard<std::mutex> lg(this->endMarkMux);
        this->endMark = 1 ;
    }
    {
        std::lock_guard<std::mutex> lg(this->mux);
        while(this->queue.size()){
            this->queue.pop();
        }
    }
}
int messageQueue::pushUserBhr(UserBhr* userbhr){
    {
        std::lock_guard<std::mutex> lg(this->mux);
        this->queue.push(userbhr);
    }
    return 1 ;  
}
UserBhr* messageQueue::popUserBhr(){
    UserBhr* ret = nullptr ; 
    {
        std::lock_guard<std::mutex> lg(this->mux);
        if(this->queue.size()){
            ret = this->queue.front();
            this->queue.pop();
        }
    }
    return ret ; 
}
