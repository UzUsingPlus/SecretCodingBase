#pragma once
#ifndef ENTITY_H
#define ENTITY_H
#include <stdio.h>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include "myConnector.h" 
extern const int GameFPS ; 
extern const int DST ; 
class SerReply;
class GameReply ; 
class UserBhr ; 


class Player {
public:
    int userId;
    string userName;
    string userPwd;
    int userdqCoin;
    int userStage ; 
    //wtf two here
    int userState;
    int readyMark ; 
    int roomId;
    int roomPos ; 
    int cliSock; 
    Player* nextPlayer;
    Player(int userId=-1, string userName="", int userdqCoin=0,int cliSock=-1,Player* nextPlayer=nullptr);
    void clearThePlayer();
};
struct PlayerInfo{
    int userId ; 
    std::string userName ; 
    int userdqCoin ; 
    int userStage ; 
    PlayerInfo(int userId=-1,std::string userName="",int userdqCoin=0,int userStage=-1);
};
struct RankElement{
    int userId ; 
    std::string userName ; 
    int score ; 
    RankElement(int userId=-1,std::string userName="",int score=0);
};
struct BulletinElement{
    int userId ; 
    std::string userName ; 
    std::string words ; 
    BulletinElement(int userId=-1,std::string userName="",std::string words="");
};

class Ball{
public:
    float radius;
    float posX;
    float posY;
    float Vx;
    float Vy;
    float Ax;
    float Ay;
public:
    Ball();    
    void clear();
}; 
class Block{
public:
    float halLength; 
    float halWidth;
    float posX; 
    float posY;
public:
    Block();  
    void clear();
};
class BallGameRoom{
private:
    int endMark ;
    int userId ;// who is playing 
    int userSocket; 
    int score ;
    Ball ball ;
    Block block ;   
    std::mutex endMarkMutex ;
    std::mutex dataMutex ; 
    std::thread calStatusThread ; 

    public:
    BallGameRoom();
    BallGameRoom(int,int); 
    BallGameRoom(BallGameRoom&& other) noexcept
:endMark(other.endMark),userId(other.userId),
userSocket(other.userSocket),score(other.score),
ball(other.ball),block(other.block)
{}
    int threadInit(); 
    void calStatus();
    void ctrlBlock(int lastUserMove);
    void detectCollision();  
    void detectMargin();
    void beginTheGame();
    void endTheGame();
    int endJudge();
    int returnEndMark();
    void buildHelper(GameReply* gr);
    void clear();
    void remake();
};
#endif // !ENTITY_H

