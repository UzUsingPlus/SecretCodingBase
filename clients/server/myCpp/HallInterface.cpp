#include "HallInterface.h"
#include "transPtc.h"
const int BHR_SUCC = 100 ; 
const int NAMEEXIST_ERROR = 400 ; 
const int FINDUSER_ERROR = 410 ; 

const int MATCH_ERROR = 420 ; 
const int EXIT_ERROR = 430 ;
const int WORD_ERROR = 440 ; 
const int ROOMID_ERROR = 450 ;
const int UNKNOWN_ERROR = 666 ; 
const int GAME_OVER = 999 ;
//
//
//
Hall::Hall(){
    this->maxGameRoomId = 0 ; 
    this->onlineUserNum = 0 ; 
    this->onlineRoomNum = 0 ; 
    this->currBleIdx = 0 ; 
    this->dummyNode = new Player(-1,"",0,-1,nullptr);
    this->ballGameRoomList.clear();
    this->bulletin.clear();
    this->bulletin.resize(0);
    this->rank.resize(0);
    this->rank = {{114,"章豪",5141},{919,"联明开",810},{999,"树脂",666}};
    this->endMark = 0 ; 
}
Hall::Hall(sql::Connection* con):
con(con)
{
    this->maxGameRoomId = 0 ; 
    this->onlineUserNum = 0 ; 
    this->onlineRoomNum = 0 ; 
    this->currBleIdx = 0 ; 
    this->dummyNode = new Player(-1,"",0,-1,nullptr);
    this->ballGameRoomList.clear();
    this->bulletin.clear();
    this->bulletin.resize(0);
    this->rank.clear();
    this->rank.resize(0);
    this->rank = {{114,"章豪",5141},{919,"联明开",810},{999,"树脂",666}};
    this->endMark = 0 ; 
}
SerReply* Hall::userRegis(string userName,string userPwd,int userSocket,int userStage){
    int code = BHR_SUCC ; 
    if (mysqlInteraction::isUserNameExist(userName,this->con)){
        std::cout << "Regis fali . The userName : " << userName << " exists" << std::endl ;  
        code = NAMEEXIST_ERROR;
    }
    mysqlInteraction::insertAcInfo(userName, userPwd, this->con);
    RegisReply* regisReply = new RegisReply();
    regisReply->fillReply(code,-1,userSocket,userStage); //code id userSocket
    regisReply->buildRegisReply(-1);//dqcoin
    return regisReply ; 
}
SerReply* Hall::userLogin(string userName,string userPwd,int userSocket,int userStage){
    int code = BHR_SUCC ; 
    if (!mysqlInteraction::isUserNameExist(userName, this->con)){
        std::cout << "Login failed . The userName : " << userName << " doesn't exists" << std::endl ;   
        code = FINDUSER_ERROR ; //can not find userName
    }else if (!mysqlInteraction::matchNameAndPwd(userName, userPwd, this->con)){
        std::cout << "Login failed . The userName : " << userName << " Can not match the username and the userpwd"<< std::endl ;   
        code = MATCH_ERROR ; 
    }
    int userId = mysqlInteraction::findIDByUserName(userName, this->con);
    int dqCoin = mysqlInteraction::findUserdqCoinByID(userId,this->con);
    Player* player = new Player(userId,userName,dqCoin,userSocket,this->dummyNode->nextPlayer);
    //this->insertNode(userId);
    {
        std::lock_guard<std::mutex> lock(this->dataMutex);
        this->insertPlayer(player); 
        this->checkPlayerList();
    }
    RegisReply* regisReply = new RegisReply();
    regisReply->fillReply(code,userId,userSocket,userStage); 
    regisReply->buildRegisReply(dqCoin);
    return regisReply ; 
}
SerReply* Hall::userLogout(int userId,int userSocket,int userStage){
    int code = BHR_SUCC ; 
    {
        std::lock_guard<std::mutex> lock(this->dataMutex);
        if(!this->delNode(userId))
            code = EXIT_ERROR ; 
        this->checkPlayerList();
    }
    HallReply* hallReply = new HallReply();
    hallReply->fillReply(code,userId,userSocket,userStage);
    hallReply->buildHallReply(-1);    
    return hallReply ; 
}
SerReply* Hall::userLeaveWords(int userId,string words,int userSocket,int userStage){
    int code = BHR_SUCC ;  
    {
        std::lock_guard<std::mutex> lock(this->dataMutex);
        cout << " show the words : " << words << endl ; 
        this->bulletin.push_back({userId,this->findNameById(userId),words});
    }
    //is there any conditions failing ?
    HallReply* hallReply = new HallReply();
    hallReply->fillReply(code,userId,userSocket,userStage);
    hallReply->buildHallReply(-1);
    this->checkBulletin();
    return hallReply ; 
}
SerReply* Hall::userEnterGame(int userId,int userSocket,int userStage){
    int code = BHR_SUCC ;     
    {
        this->ballGameRoomList.emplace_back(userId,userSocket);
    }
    HallReply* hallReply = new HallReply();
    hallReply->fillReply(code,userId,userSocket,userStage);
    hallReply->buildHallReply(this->ballGameRoomList.size()-1);
    this->maxGameRoomId ++ ; 
    return hallReply ;  
}

//alright under the line no lock_guard yet
//
//
//
SerReply* Hall::userBeginGame(int userId,int roomId,int userSocket,int userStage){
    int code = BHR_SUCC ; 
    GameReply* gameReply = new GameReply();
    if( roomId >= this->ballGameRoomList.size() ){
        code = ROOMID_ERROR ; 
        gameReply->fillReply(code,userId,userSocket,userStage);
        //gameReply->buildGameReply(bgr.endMark,bgr.score,bgr.ball,bgr.block);
        return gameReply  ; 
    }
    BallGameRoom& bgr = this->ballGameRoomList[roomId];
    bgr.beginTheGame();
    gameReply->fillReply(code,userId,userSocket,userStage);
    bgr.buildHelper(gameReply);
    //gameReply->buildGameReply(bgr.endMark,bgr.score,bgr.ball,bgr.block);
    return gameReply ; 
}
SerReply* Hall::userCtrlGame(int userId,int roomId,int userSocket,int userStage,int moveDir){
    int code = BHR_SUCC;
    GameReply* gameReply = new GameReply();
    //to do : deal with the moveDir and new a reply to send to usr.
    if( roomId >= this->ballGameRoomList.size() ){
        code = ROOMID_ERROR ; 
        gameReply->fillReply(code,userId,userSocket,userStage);
        //gameReply->buildGameReply(bgr.endMark,bgr.score,bgr.ball,bgr.block);
        return gameReply  ; 
    }
    BallGameRoom& bgr = this->ballGameRoomList[roomId];
    // the moveDir? how to update the block info? todo
    bgr.calBlockStatus(moveDir);
    gameReply->fillReply(code,userId,userSocket,userStage);
    bgr.buildHelper(gameReply);
    //gameReply->buildGameReply(bgr.endMark,bgr.score,bgr.ball,bgr.block);
    return gameReply ; 
}
SerReply* Hall::userLeaveGame(int userId,int roomId,int userSocket,int userStage){
    cout << "try to deal with it ." << endl ;  
    int code = BHR_SUCC ; 
    GameReply* gameReply = new GameReply();
    if( roomId >= this->ballGameRoomList.size() ){
        code = ROOMID_ERROR ; 
        gameReply->fillReply(code,userId,userSocket,userStage);
        return gameReply  ;  
    }
    cout << " has fill the reply" << endl ; 
    BallGameRoom& bgr = this->ballGameRoomList[roomId];
    bgr.endTheGame();
    gameReply->fillReply(code,userId,userSocket,userStage);
    bgr.buildHelper(gameReply);
    //gameReply->buildGameReply(bgr.endMark,bgr.score,bgr.ball,bgr.block);
    bgr.clear();
    return gameReply ; 
}
vector<HallBroadCastReply*> Hall::buildHBCRVec(){
    std::vector<HallBroadCastReply*> HBCRVec ;
    if(dummyNode == nullptr) 
        return HBCRVec ; 
    Player* curr = this->dummyNode->nextPlayer ;
    {
        std::lock_guard<std::mutex> lock(this->dataMutex);     
        std::vector <PlayerInfo> playerList;
        while( curr != nullptr ){
            playerList.push_back({curr->userId,curr->userName,curr->userdqCoin,curr->userStage});
            curr = curr->nextPlayer ; 
        } 
        curr = this->dummyNode->nextPlayer ; 
        while( curr != nullptr ){
            HallBroadCastReply* hbcr = nullptr ; 
            if(hbcr = new (std::nothrow)HallBroadCastReply()){
                hbcr->fillReply(BHR_SUCC,curr->userId,curr->cliSock,HALL_BROADCAST);
                cout << this->currBleIdx << endl ; 
                cout << this->bulletin[this->currBleIdx].userId << " " << this->bulletin[this->currBleIdx].userName << " " << this->bulletin[this->currBleIdx].words << endl ; 
                hbcr->buildHallBroadCastReply(playerList,this->rank,this->bulletin[this->currBleIdx]);
                HBCRVec.emplace_back(hbcr); 
                curr = curr->nextPlayer ; 
            }
            else {
                cout << "opps ! memory allot failed!" << endl ; 
                return {} ; 
            }
        }//deal with the currBleIdx 
        cout << "the currBleIdx: "  << this->currBleIdx << " and the size: " << this->bulletin.size() << endl ; 
        if(this->currBleIdx == this->bulletin.size()-1)
            this->currBleIdx = 0 ;
        else if( this->bulletin.size() == 0 ){

        }else
            this->currBleIdx ++ ;  
    }
    return HBCRVec ; 
    
}
int Hall::insertNode(int userId){
    Player* newPlayer = new Player();
    newPlayer->userId = userId ; 
    newPlayer->nextPlayer = this->dummyNode->nextPlayer ;
    this->dummyNode->nextPlayer = newPlayer ;
    return 1 ; 
}
Player* Hall::findNode(int userId){
    Player* curr = this->dummyNode ; 
    while(curr){
        if(curr->userId == userId)
            break ; 
        else 
            curr = curr->nextPlayer ;    
    }  
    return curr ; 
}
int Hall::delNode(int userId){
    Player* curr = this->dummyNode ; 
    while(curr->nextPlayer){
        if(curr->nextPlayer->userId == userId){
            Player* toDel = curr->nextPlayer ;
            curr->nextPlayer = toDel->nextPlayer ; 
            delete toDel ;  
            return 1; //delete success
        }
    }
    return 0; //can not find it out
}
int Hall::findSocketById(int userId){
    Player* curr = this->findNode(userId);
    if( curr ) 
        return curr->cliSock ; 
    else 
        return -1; 
}
string Hall::findNameById(int userId){
    Player* curr = this->findNode(userId);
    if( curr )
        return curr->userName;  
    else
        return "error" ;
}
void Hall::checkPlayerList(){
    if(!this->dummyNode)
        cout << "dummyNode is nullptr" << endl ; 
    Player* curr = this->dummyNode ; 
    while( curr ) {
        cout << "userId:" <<curr->userId <<" "<< "userName:" << curr->userName << "userSocket:" << curr->cliSock <<" " << "userdqCoin:" << curr->userdqCoin <<endl ; 
        curr = curr->nextPlayer ; 
    }
}
void Hall::checkBulletin(){
    for(BulletinElement &be : this->bulletin){
        cout << be.userId << " " << be.userName << be.words << endl ; 
    }
}
int Hall::insertPlayer(Player* player){
    player->nextPlayer = this->dummyNode->nextPlayer ; 
    this->dummyNode->nextPlayer = player ; 
    return 1;  
}
