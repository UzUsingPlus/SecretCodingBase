#pragma once
#ifndef ENTITY_H
#define ENTITY_H
#include <stdio.h>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

using namespace std ;
class Player {
public:
    int userId;
    string userName;
    int userdqCoin;
    int userStage;
    Player(); 
    Player(int userId, string userName, int userdqCoin,int userState);
    void clearThePlayer();
};
struct RankElement{
    int userId;
    std::string userName;
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
public:
    Ball(float radius = 5 , float posX = 320,float posY = 25);
}; 
class Block{
public:
    float halLength; 
    float halWidth;
    float posX; 
    float posY;
public:
    Block(float halLength = 20 ,float halWidth = 5 , float posX = 300,float posY = 470);
};
class BallGameRoom{
public:
    volatile int endMark ;
    std::mutex updateMutex ;
    int userId ;// who is playing 
    int userSocket; 
    int score ;  
    Ball ball ;
    Block block ;
public:
    BallGameRoom();
    int returnTheScore();
    void beginTheGame();
    void endTheGame();
    void upDateRoomInfo(int,int,Ball&,Block&);
};
#endif
