#pragma once 
#ifndef TRANSPTC_H
#define TRANSPTC_H
#include "entity.h"
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include <string>
#include <fstream>
#include <vector>
#include "json.hpp"
#include "json_fwd.hpp"
#include <condition_variable>
#include <memory>
using json = nlohmann::json ; 
extern const int REGIS_BHR ; 
extern const int LOGIN_BHR ;
extern const int LOGOUT_BHR ;
extern const int LEAVE_WORDS_BHR ;
extern const int ENTER_GAME_BHR ;
extern const int BEGIN_GAME_BHR ;
extern const int CTRL_GAME_BHR ;
extern const int LEAVE_GAME_BHR ;   
extern const int UPDATE_HALL_INFO_BHR ; 
extern const int UPDATE_GAME_INFO_BHR ; 
extern const int HALL_BROADCAST ; 
class Player ; 
class Ball ; 
class Block ;
class BallGameRoom ; 
struct RankElement ; 
struct BulletinElement ; 
class HallInterface ; 
class Hall ; 

struct SerReply{  
    int code ; 
    int userId ;
    int userSocket ; //..? 
    int userStage ; 
    int fillReply(int,int,int,int);// code userId userSocket userStage 
    virtual ~SerReply() = default ; 
    virtual json analyzeStructToJson() = 0;
}; 
struct RegisReply : public SerReply{
    int dqCoin ; //when roll in
    int buildRegisReply(int); 
    json analyzeStructToJson()override; 
};
struct HallReply : public SerReply{
    int roomId ;
    int buildHallReply(int);
    json analyzeStructToJson()override; 
};
struct GameReply : public SerReply{
    int endMark ; 
    int score ; 
    //std::unique_ptr<Ball> ballPtr;
    //std::unique_ptr<Block> blockPtr;
    Ball ball ; 
    Block block ; 
    json analyzeStructToJson()override; 
    int buildGameReply(int,int,Ball,Block);
};
struct HallBroadCastReply : public SerReply{
    std::vector<PlayerInfo>playerList;
    std::vector<RankElement>rank;
    //std::vector<BulletinElement>bulletin;
    BulletinElement currBle ; 
    json analyzeStructToJson()override; 
    int buildHallBroadCastReply(std::vector<PlayerInfo>,std::vector<RankElement>,BulletinElement);
};
void to_json(json& j,const PlayerInfo &p);
void to_json(json& j,const RankElement& re);
void to_json(json& j,const BulletinElement& be);
void to_json(json& j,const HallBroadCastReply& hr);

struct UserBhr{
    int code ; 
    int userId ;
    int userSocket ; 
    int userStage ; 
    std::string addtion ;
    virtual ~UserBhr();
    UserBhr() ;
    UserBhr(int code =-1,int userId=-1,int userSocket=-1,int userStage=-1,std::string addtion="");
    virtual void analyzeJsonToStruct(json js) = 0 ;
    virtual SerReply* dealBhr(HallInterface* hall) = 0 ;     
};
 struct RegisBhr : public UserBhr{
    using UserBhr::UserBhr;
    std::string userName ; 
    std::string userPwd ; 
    RegisBhr():UserBhr(-1,-1,-1,-1,""){};
    void analyzeJsonToStruct(json js) override;
    SerReply* dealBhr(HallInterface* hall) override;
 };
 struct HallBhr : public UserBhr{
    using UserBhr::UserBhr;
    std::string words ; 
    HallBhr():UserBhr(-1,-1,-1,-1,""){};
    void analyzeJsonToStruct(json js) override;
    SerReply* dealBhr(HallInterface* hall) override;
};
struct GameBhr : public UserBhr{
    using UserBhr::UserBhr;
    int roomId ; 
    int moveDir ; 
    GameBhr():UserBhr(-1,-1,-1,-1,""){};
    void analyzeJsonToStruct(json js)override;
    SerReply* dealBhr(HallInterface* hall)override;
}; 
//
//
//
class replyQueue{
    private:
        std::queue<SerReply*> queue ;
        std::mutex mux ;
    public:
        replyQueue();
        int pushSerReply(SerReply* serReply);
        SerReply* popSerReply();
    };
class messageQueue{
private:
    std::queue <UserBhr*> queue ;
    std::mutex mux ;
    std::mutex endMarkMux;
    volatile int endMark ;
    std::thread dealThread ;  
    HallInterface* hall ; 
    replyQueue* rq ; 
    
public:
    messageQueue(Hall* hall = nullptr , replyQueue* rq = nullptr);
    int pushUserBhr(UserBhr* userbhr);
    UserBhr* popUserBhr(); 
    int threadInit(); 
    int dealConstantly();
    SerReply* replyQueuePop();
};
struct Packet{
    uint32_t data_len; 
    char data[]; 
};
#endif
