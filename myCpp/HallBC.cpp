#include "HallBC.h"
HallBC::HallBC(){
    this->endMark = 0 ;
    this->threadInit();
}
HallBC::~HallBC(){
    {
        std::lock_guard<std::mutex> lock(this->endMarkmutex);
        this->endMark = 1 ; 
    }
    this->HallBCThread.join();  
}
HallBC::HallBC(Hall* hall , replyQueue* rq)
:hall(hall),rq(rq)
{
    this->endMark = 0 ; 
    this->threadInit();
}
void HallBC::threadInit(){
    this->HallBCThread = std::thread(&HallBC::HallBroadCast,this);
}
void HallBC::HallBroadCast(){
    while(!(this->endMark))
    {
        //should we take the lock_guard .. ?
        //std::lock_guard<std::mutex> lock(this->dataMutex);
        cout << "Ready to HallBroadCast" << endl ;
        try{
        vector<HallBroadCastReply*> hbcrVec = this->hall->buildHBCRVec();
        for(int i = 0 ; i < hbcrVec.size() ; i++ ){
            this->rq->pushSerReply(hbcrVec[i]);
        }    
        cout << "Finish HallBroadCast . Ready to sleep." << endl ; 
        }
        catch(...){
            
        }
        std::this_thread::sleep_for(std::chrono::seconds(30));//1 bc per min
    }  
}