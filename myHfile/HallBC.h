#include "HallInterface.h"
#include "transPtc.h"
class HallBC {
private:    
    int endMark  ; 
    std::mutex endMarkmutex ; 
    std::mutex dataMutex ; 
    Hall *hall ; 
    replyQueue *rq ;
    std::thread HallBCThread ;
public :
    HallBC();
    ~HallBC();
    HallBC(Hall* hall = nullptr,replyQueue* rq = nullptr);
    void HallBroadCast();
    void threadInit();
    
}; 