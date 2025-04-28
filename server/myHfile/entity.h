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
    Player(); 
    Player(int userId, string userName, int userdqCoin, Player* nextPlayer);
    void clearThePlayer();
};
struct RankElement{
    int userId ; 
    std::string userName ; 
    int score ; 
};
struct BulletinElement{
    int userId ; 
    std::string userName ; 
    std::string words ; 
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
    void checkMargin();
    void clear();
}; 
class Block{
public:
    float halLength; 
    float halWidth;
    float posX; 
    float posY;
public:
    void clear();
};
class BallGameRoom{
// public :
//     BallGameRoom(const BallGameRoom&) = delete ;
//     BallGameRoom& operator = (const BallGameRoom&) = delete ;
//     BallGameRoom(BallGameRoom&&) = default ; 
//     BallGameRoom& operator = (BallGameRoom&&)=default ;
private:
    volatile int endMark ;
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
    void calBlockStatus(int lastUserMove);
    void detectCollision();  
    void beginTheGame();
    void endTheGame();
    void clear();
    void buildHelper(GameReply* gr);
};
#endif // !ENTITY_H

