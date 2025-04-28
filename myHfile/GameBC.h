#include "HallInterface.h"
#include "transPtc.h"
#include <mutex>
extern const int SEND_GR_T ; 
class GameBC{
private:
    Hall* hall ;
    replyQueue* rq ;       
    int endMark ;
    std::mutex endMarkMutex ; 
    std::thread gameBroadCastThread ; 
public:
    GameBC(Hall* hall = nullptr,  replyQueue* rq=nullptr); 
    ~GameBC();
    void gameBroadCast();
    void threadInit();
};