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
#include "json.hpp"
#include "json_fwd.hpp"
#include <condition_variable>
using json = nlohmann::json ;
//userStage
extern const int REGIS_BHR ; 
extern const int LOGIN_BHR ;
extern const int LOGOUT_BHR ;
extern const int LEAVE_WORDS_BHR ;
extern const int UPDATE_HALL_INFO_BHR;
extern const int ENTER_GAME_BHR ;
extern const int BEGIN_GAME_BHR ;
extern const int CTRL_GAME_BHR ;
extern const int UPDATE_GAME_INFO_BHR;
extern const int LEAVE_GAME_BHR ;
//code
extern const int BHR_SUCC ;
extern const int NAMEEXIST_ERROR ;
extern const int FINDUSER_ERROR ;
extern const int MATCH_ERROR ;
extern const int EXIT_ERROR ;
extern const int WORD_ERROR ;
extern const int ROOMID_ERROR  ;
extern const int UNKNOWN_ERROR ;
extern const int GAME_OVER ;
class HallInterface ;
class Hall ;

struct SerReply{//how to deal with it ?
    int code ; 
    int userId ;
    int userSocket ; //..?
    int userStage ;//no need for construct function
    void fillBasicInfo(json js);
    virtual ~SerReply(){}
    virtual void dealReply(HallInterface* hall) = 0 ;
    virtual void analyzeJsonToStruct(json js) = 0 ;
};
struct RegisReply : public SerReply{
    int dqCoin ; //when roll in
    void dealReply(HallInterface* hall)override;
    void analyzeJsonToStruct(json js)override;
};
struct HallReply : public SerReply{
    int roomId ;
    void dealReply(HallInterface* hall)override;
    void analyzeJsonToStruct(json js)override ;
};
struct GameReply : public SerReply{
    int endMark ;
    int score ;
    Ball ball ;
    Block block ;
    void dealReply(HallInterface* hall)override;
    void analyzeJsonToStruct(json js)override ;
};
struct HallBroadCastReply: SerReply{
    string currWords ;
    vector<Player> playerList ;
    BulletinElement currBle ;
    //vector<BulletinElement> bulletin ;
    vector<RankElement> rank ;
    void dealReply(HallInterface* hall)override ;
    void analyzeJsonToStruct(json js)override ;
};
// void from_json( Player& p,json &js);
// void from_json(RankElement& re,json &js);
// void from_json( BulletinElement& be,json &js);
// void from_json( HallBroadCastReply& hr,json &js);
void from_json(const json &js, Player& p);
void from_json(const json &js,RankElement& re);
void from_json(const json &js , BulletinElement& be);
void from_json(const json &js, BulletinElement& hr);
struct UserBhr{//code is NOW useless in the UserBhr
    int code ;
    int userId ;
    int userSocket ;
    int userStage ; //BhrCode as well
    std::string addtion ;
    virtual ~UserBhr(){}
    virtual json struct_to_json() = 0 ;
    UserBhr(int code = -1,int userId=-1,int userSocket=-1,int userStage=-1,string addition="");
    int fillBhr(int userId,int userSocket,int userStage);
};
 struct RegisBhr : public UserBhr{
    using UserBhr::UserBhr;
    std::string userName ; 
    std::string userPwd ;
    json struct_to_json()override ;
    void buildRegisBhr(string,string);
 };
 struct HallBhr : public UserBhr{
    using UserBhr::UserBhr;
    std::string words ;
    json struct_to_json()override;
    void buildHallBhr(string);
};
struct GameBhr : public UserBhr{
    using UserBhr::UserBhr;
    int roomId ; 
    int moveDir;
    json struct_to_json()override;
    void buildGameBhr(int,int);
}; 
//
//
class replyQueue{
    private:
        std::queue<SerReply*> queue ;
        std::mutex mux ;
        std::mutex endMarkMux;
        volatile int endMark ;
        std::thread dealThread ;
    public:
        Hall* hall ;
        replyQueue();
        ~replyQueue();
        int pushSerReply(SerReply* serReply);
        SerReply* popSerReply();
        int threadInit();
        int dealConstantly();
};
class messageQueue{
private:
    std::queue <UserBhr*> queue ;
    std::mutex mux ;
    std::mutex endMarkMux;
    volatile int endMark ;
public:
    messageQueue();
    ~messageQueue();
    int pushUserBhr(UserBhr* userbhr);
    UserBhr* popUserBhr(); 
};
#endif
