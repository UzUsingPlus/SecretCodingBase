#pragma once 
#ifndef HALLINTERFACE_H
#define HALLINTERFACE_H
#include "entity.h"
//#include "transPtc.h"
#include <string>   
#include <mutex>
extern const int BHR_SUCC ; 
extern const int NAMEEXIST_ERROR ; 
extern const int FINDUSER_ERROR ; 
extern const int MATCH_ERROR ; 
extern const int EXIT_ERROR ;
extern const int WORD_ERROR ; 
extern const int ROOMID_ERROR ;
extern const int UNKNOWN_ERROR ; 
extern const int GAME_OVER ;
 
class UserBhr;
class SerReply; 
class messageQueue ;
class replyQueue ;
class HallInterface{
public:
    virtual ~HallInterface(){}
    virtual SerReply* userRegis(string,string,int,int) = 0;
    virtual SerReply* userLogin(string,string,int,int) = 0;
    virtual SerReply* userLogout(int,int,int) = 0 ;
    virtual SerReply* userLeaveWords(int,string,int,int) = 0 ;
    virtual SerReply* userEnterGame(int,int,int) = 0 ;
    virtual SerReply* userBeginGame(int,int,int,int) = 0 ;// userId , roomId , userSocket , userStage    
    virtual SerReply* userCtrlGame(int,int,int,int,int) = 0 ; //we just no care about the CODE in the userReply.
    virtual SerReply* userLeaveGame(int,int,int,int) = 0 ;
};
//be careful that dont define it twice
class Hall : public HallInterface{
private:
    //Player* readyNode;//the player in the regis 
    int maxGameRoomId ;//init as zero.  
    int onlineUserNum ; 
    int onlineRoomNum ; 
    Player* dummyNode;
    vector<BallGameRoom> ballGameRoomList;
    vector<BulletinElement> bulletin ; //ABOVE:data structure to store the data !
    vector<RankElement> rank ; 
    int endMark ; 
    std::mutex endMarkMutex ;

public: 
    Hall();
    Hall(sql::Connection* con );  
    SerReply* userRegis(string,string,int,int) override;
    SerReply* userLogin(string,string,int,int) override;
    SerReply* userLogout(int,int,int) override; 
    SerReply* userLeaveWords(int,string,int,int) override ; 
    SerReply* userEnterGame(int,int,int) override ;
    SerReply* userBeginGame(int,int,int,int) override ;
    SerReply* userCtrlGame(int,int,int,int,int) override ; 
    SerReply* userLeaveGame(int,int,int,int) override ; 

    //the list operation : 
    int insertNode(int userId);
    Player* findNode(int userId);
    int delNode(int userId);
    int findSocketById(int userId);
    string findNameById(int userId);    
    sql::Connection* con;    
};
#endif
