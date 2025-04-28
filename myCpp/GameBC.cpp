#include "GameBC.h"
const int SEND_GR_T = 500 ; //ms 
GameBC::GameBC(Hall* hall,replyQueue* rq)
:hall(hall),rq(rq)
{
    {
        std::lock_guard<std::mutex> lock(this->endMarkMutex);
        this->endMark = 1 ; 
    }
    this->threadInit();
}
GameBC::~GameBC(){
    {
        std::lock_guard<std::mutex> lock(this->endMarkMutex);
        this->endMark = 1 ; 
    }
    if(this->gameBroadCastThread.joinable())
        this->gameBroadCastThread.join();
}
void GameBC::gameBroadCast(){
    while(!(this->endMark))
    {
        {
            std::lock_guard<std::mutex> lock(this->endMarkMutex);
            vector<GameReply*> GRVec = this->hall->buildGRVec(); 
            for(GameReply* gr : GRVec){
                this->rq->pushSerReply(gr);
            }
        }
        std::this_thread::sleep_for(std::chrono::microseconds(SEND_GR_T));
//        std::this_thread::sleep_for(std::chrono::milliseconds(SEND_GR_T));
    }
}
void GameBC::threadInit(){
    {
        std::lock_guard<std::mutex> lock(this->endMarkMutex);
        this->endMark = 0 ;
    }
    this->gameBroadCastThread = std::thread(&GameBC::gameBroadCast,this);
}