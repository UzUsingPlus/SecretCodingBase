#pragma once
#ifndef HALLINTERFACE_H
#define HALLINTERFACE_H
#include "entity.h"
#include "transPtc.h"
#include <QObject>
#include <string>
#include <mutex>
#include <QString>
extern const int MOVE_L;
extern const int MOVE_R;
class GameUi ;
class LoginUi ;
class UserBhr;
class SerReply;
class messageQueue ;
class replyQueue ;
class HallInterface : public QObject{
public:
    virtual ~HallInterface(){}
    virtual void regisReply(int) = 0 ;
    virtual void loginReply(int,int,int,int) = 0 ;//need userId and dqCoin
    virtual void logoutReply(int) = 0 ;
    virtual void leaveWordsReply(int) = 0 ;
    virtual void enterGameReply(int,int) = 0 ;//need roomId
    virtual void beginGameReply(int) = 0 ;
    virtual void leaveGameReply(int) = 0 ;

    virtual void updateHallInfo(int,vector<Player>&,BulletinElement&,vector<RankElement>&) = 0  ;
    virtual void updateGameInfo(int,int,int,Ball,Block) = 0 ;

    virtual RegisBhr* buildRegisBhr() = 0 ;
    virtual RegisBhr* buildLoginBhr() = 0 ;
    virtual HallBhr* buildLogoutBhr() = 0 ;
    virtual HallBhr* buildLeaveWordsBhr(string) = 0 ;
    virtual HallBhr* buildEnterGameBhr() = 0 ;
    virtual GameBhr* buildBeginGameBhr() = 0 ;
    virtual GameBhr* buildCtrlGameBhr(int) = 0 ;
    virtual GameBhr* buildLeaveGameBhr() = 0 ;

};
//be careful that dont define it twice
class Hall : public HallInterface{
    Q_OBJECT
private:
    int localUserId ;
    string localUserName ;
    string localUserPwd ;
    int localUserSocket ;
    int localUserDqCoin ;
    int localRoomId ;

    BallGameRoom localRoom ;
    int maxGameRoomId ;//init as zero.
    int onlineUserNum ;
    int onlineRoomNum ;
    int currWordsIdx;
    vector<Player> playerList ;
    vector<RankElement> rank ;//rank!
    BulletinElement currBle ;

    int updateMark ;
    std::mutex updateMarkMutex ;
    int endMark ;
    std::mutex endMarkMutex ;

    std::thread GameListenKB ;
    int gameEndMark ;
    std::mutex gameEndMarkMutex ;

    messageQueue *mq ; //only useful when user have click something and need to send a request ..

public:
    Hall();
    Hall(messageQueue* mq);
    void fillNamePwd(string,string);
    void Pushmq(UserBhr*);
    vector<QString> rankToQString();
    QString wordsToQString();
    //normal function
    virtual void regisReply(int)override;
    virtual void loginReply(int,int,int,int)override;
    virtual void logoutReply(int)override;
    virtual void leaveWordsReply(int)override;
    virtual void enterGameReply(int,int)override;
    virtual void beginGameReply(int)override;
    virtual void leaveGameReply(int)override;
    //broadCastFunction
    void updateHallInfo(int,vector<Player>& ,BulletinElement&,vector<RankElement>&)override;
    // void updateHallInfo(int,vector<Player>&,vector<BulletinElement>&,vector<RankElement>&)override;
    void updateGameInfo(int,int,int,Ball,Block) override;

    RegisBhr* buildRegisBhr()override;
    RegisBhr* buildLoginBhr()override ;

    HallBhr* buildLogoutBhr()override;
    HallBhr* buildLeaveWordsBhr(string words)override;
    HallBhr* buildEnterGameBhr()override;

    GameBhr* buildBeginGameBhr()override;
    GameBhr* buildCtrlGameBhr(int moveDir)override;
    GameBhr* buildLeaveGameBhr()override;
    void listenKB();
    void calBlockPara(int& x,int& y,int& length,int& width);
    void calBallPara(int& x,int& y,int& radius);
    int returnTheGameScore() ;
    int judgeGameOver();
    //the list operation :



    void LoginToHall();
    void HallToLogin();
    void HallToGame();
    void GameToHall();

signals :
    void regisSlot(int code);
    void loginSlot(int code);
    void logoutSlot(int code);
    void leaveWordsSlot(int code);
    void enterGameSlot(int code);
    void beginGameSlot(int code);
    void leaveGameSlot(int code);
    void updateHallSlot(int code);
    void updateGameSlot(int code);
    void endTheGameSlot(int code);
};
#endif // HALLINTERFACE_H
