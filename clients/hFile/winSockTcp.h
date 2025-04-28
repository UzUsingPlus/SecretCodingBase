#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#ifndef WINSOCKTCP_H
#define WINSOCKTCP_H
#define NOMINMAX
#include "transPtc.h"
#include <unordered_map>
#include <thread>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <iostream>
#include <sstream>
#include <time.h>
#include <cstddef>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib,"ws2_32.lib")
#undef byte
extern const int MAX_BUFFER_SIZE;
class winSockTcp{
private:
    WSADATA wsadata;
    SOCKET cliSock;
    vector<string> localIP;
    string serIP;
    SOCKADDR_IN sockAddr;
    replyQueue *rq ;
    messageQueue *mq ;
    std::thread recvThread ;
    std::thread sendThread ;
    volatile int endMark ;
public:
    winSockTcp();
    winSockTcp(replyQueue* rq,messageQueue* mq);
    string jsonToString(json);
    json stringToJson(string);
    int  initConnection();
    void threadInit();
    void getLocalIP();
    void shutdown();

    int Send4Res(string data);
    int Send(string data);
    void sendConstantly();
    json UserBhrToJson(UserBhr* userBhr);

    int Recv(string &data);
    int RecvInfo(string &data);
    void recvConstantly();
    SerReply* jsonToSerReply(json js);


};
#endif // WINSOCKTCP_H
