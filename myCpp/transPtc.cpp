#include "transPtc.h"
#include "HallInterface.h"
#include "entity.h"
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include <condition_variable>
const int REGIS_BHR = 10 ; 
const int LOGIN_BHR = 20 ;
const int LOGOUT_BHR = 110 ;
const int LEAVE_WORDS_BHR = 120 ;
const int ENTER_GAME_BHR = 130;
const int BEGIN_GAME_BHR = 210;
const int LEAVE_GAME_BHR = 220 ;
const int UPDATE_GAME_INFO_BHR = 230 ;     
const int CTRL_GAME_BHR = 240 ; 
const int HALL_BROADCAST = 300 ;
 
using namespace std ;  

UserBhr::UserBhr(){
    this->userStage = -1;
    this->userId = -1; 
    addtion = "";
}
UserBhr::~UserBhr(){}
UserBhr::UserBhr(int code,int userId,int userSocket,int userStage,std::string addtion)
:code(code),userId(userId),userSocket(userSocket),userStage(userStage),addtion(addtion)
{ 
}
void RegisBhr::analyzeJsonToStruct(json js){
    this->code = js["code"];
    this->userId = js["userId"];
    this->userSocket = js["userSocket"];
    this->userStage = js["userStage"];

    this->userName = js["userName"];
    this->userPwd = js["userPwd"];
}
void HallBhr::analyzeJsonToStruct(json js){
    this->code = js["code"];
    this->userId = js["userId"];
    this->userSocket = js["userSocket"];
    this->userStage = js["userStage"];

    this->words = js["words"];
}
void GameBhr::analyzeJsonToStruct(json js){
    this->code = js["code"];
    this->userId = js["userId"];
    this->userSocket = js["userSocket"];
    this->userStage = js["userStage"];

    this->roomId = js["roomId"];
    this->moveDir = js["moveDir"]; 
}
SerReply* RegisBhr::dealBhr(HallInterface* hall){
    int bhrCode = this->userStage ; 
    if( bhrCode ==  REGIS_BHR ){
        return hall->userRegis(this->userName,this->userPwd,this->userSocket,this->userStage);
    }else if( bhrCode == LOGIN_BHR){
        return  hall->userLogin(this->userName,this->userPwd,this->userSocket,this->userStage);
    }else{
        return nullptr;  
        //nothing to do... ?
    }
}
SerReply* HallBhr::dealBhr(HallInterface* hall){
    int bhrCode = this->userStage ;
    if( bhrCode == LOGOUT_BHR ){
        //cout << "user loggout " << endl ; 
        return hall->userLogout(this->userId,this->userSocket,this->userStage);
    }else if ( bhrCode == LEAVE_WORDS_BHR ){
        //cout << "user leave words "<< endl  ; 
        SerReply* sr = hall->userLeaveWords(this->userId,this->words,this->userSocket,this->userStage) ;
        //HallReply* hallReply = dynamic_cast<HallReply*>(sr);
        //cout << hallReply->code << " " << hallReply->userId << " " << hallReply->userSocket << " " << hallReply->roomId << endl ; 
        return sr ;  
        return hall->userLeaveWords(this->userId,this->words,this->userSocket,this->userStage);
    }else if ( bhrCode == ENTER_GAME_BHR ){
        return hall->userEnterGame(this->userId,this->userSocket,this->userStage);
    }else{
        return nullptr ; 
    }
}
SerReply* GameBhr::dealBhr(HallInterface* hall){
    int bhrCode = this->userStage ; 
    if(bhrCode == BEGIN_GAME_BHR){
        return hall->userBeginGame(this->userId,this->roomId,this->userSocket,this->userStage); 
    }else if(bhrCode == LEAVE_GAME_BHR ){
        return hall->userLeaveGame(this->userId,this->roomId,this->userSocket,this->userStage); 
    }else if(bhrCode == CTRL_GAME_BHR){
        return hall->userCtrlGame(this->userId,this->roomId,this->userSocket,this->userStage,this->moveDir); 
    }else{
        return nullptr ;  
    }
}
int SerReply::fillReply(int code,int userId,int userSocket,int userStage){
    this->code = code ; 
    this->userId = userId ;
    this->userSocket = userSocket ;  
    this->userStage = userStage ; 
    return 1;   
}
json RegisReply::analyzeStructToJson(){
    json js =  json{
                    {"code",this->code},
                    {"userId",this->userId},
                    {"userSocket",this->userSocket},
                    {"userStage",this->userStage},
                    {"dqCoin",this->dqCoin}
                }; 
    return js ; 
}
int RegisReply::buildRegisReply(int dqCoin){
    this->dqCoin = dqCoin ; 
    return 1; 
}
json HallReply::analyzeStructToJson(){
    json js =  json{
                    {"code",this->code},
                    {"userId",this->userId},
                    {"userSocket",this->userSocket},
                    {"userStage",this->userStage},
                    {"roomId",this->roomId}
                };
    return js ;  
}
int HallReply::buildHallReply(int roomId){
    this->roomId = roomId ;  
    return 1 ;  
}
json GameReply::analyzeStructToJson(){
    json js =  json{
                    {"code",this->code},
                    {"userId",this->userId},
                    {"userSocket",this->userSocket},
                    {"userStage",this->userStage},

                    {"endMark",this->endMark},
                    {"score",this->score},
                    {"ball",{
                                {"radius",this->ball.radius},
                                {"posX",this->ball.posX},
                                {"posY",this->ball.posY}
                            }
                    },
                    {"block",{
                                {"halLength",this->block.halLength},
                                {"halWidth",this->block.halWidth},
                                {"posX",this->block.posX},
                                {"posY",this->block.posY}
                            }
                    }
                }; 
    return js ; 
}
int GameReply::buildGameReply(int endMark,int score,Ball ball,Block block){
    this->endMark = endMark ; 
    this->score = score ; 
    this->ball = ball ; 
    this->block = block ; 
    return 1 ;  
}
void to_json(json &j , const PlayerInfo& p){
    j = {
        {"userId",p.userId},
        {"userName",p.userName},
        {"userdqCoin",p.userdqCoin},
        {"userStage",p.userStage}
    };
}
void to_json(json &j , const RankElement& re){
    j = {
        {"userId",re.userId},
        {"userName",re.userName},
        {"score",re.score}
    };
}
void to_json(json &j , const BulletinElement& be){
    j = {
        {"userId",be.userId},
        {"userName",be.userName},
        {"words",be.words}
    };
}
void to_json(json &j,const HallBroadCastReply& hr){
    j["code"] = hr.code ; 
    j["userId"] = hr.userId ;
    j["userSocket"] = hr.userSocket ; 
    j["userStage"] = hr.userStage ; 
    j["playerList"] = hr.playerList ; 
    j["rank"]  = hr.rank ; 
    j["currBle"] = hr.currBle;
}

json HallBroadCastReply::analyzeStructToJson(){
    json js ; 
    to_json(js,*this);
    return js ; 
}
int HallBroadCastReply::buildHallBroadCastReply(vector<PlayerInfo>playerList,vector<RankElement>rank,BulletinElement currBle){
    this->playerList = playerList ; 
    this->rank = rank ; 
    this->currBle = currBle ; 
    return 1 ;  
}
replyQueue::replyQueue(){}
int replyQueue::pushSerReply(SerReply* serReply){
    {
        std::lock_guard<std::mutex> lg(this->mux);
        this->queue.push(serReply);
    }
    return 1;  
}
SerReply* replyQueue::popSerReply(){// default the ret is nullptr  and if the queue dont have any ele , just return nullptr 
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
messageQueue::messageQueue(Hall * hall , replyQueue* rq){
    this->hall = hall ; 
    this->rq = rq ; 
    this->threadInit();  
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
int messageQueue::threadInit(){
    this->dealThread = std::thread(&messageQueue::dealConstantly,this); 
    return 1 ; 
}
int messageQueue::dealConstantly(){
    while(!(this->endMark)){
        UserBhr* userBhr = this->popUserBhr(); //userBhr -> serReply 
        if( userBhr != nullptr ){
            this->rq->pushSerReply(userBhr->dealBhr(this->hall));    
        }
        else{
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
    return 1 ; 
}
SerReply* messageQueue::replyQueuePop(){
    return this->rq->popSerReply();
}

