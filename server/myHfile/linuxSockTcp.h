#pragma once
#ifndef LINUXSOCKTCP_H
#define LINUXSOCKTCP_H
#include "transPtc.h"
#include "entity.h"
#include "json.hpp"
#include "json_fwd.hpp"
#include <fstream>
#include <chrono>
#include <iostream>
#include <thread>
#include <cstring>
#include <mutex>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <sys/epoll.h>
#include <string>
#define maxOnline 30 
#define maxEvents 10
using namespace std;
class linuxSockTcp {
private:
	int errGap ; // if sending failed , the gap to resend.
	int server_fd , epoll_fd ;
	int client_fd[maxOnline];
	int onlineNum ; 
	volatile int endMark ; 
	std::mutex mux ; //to protect endMark
	std::thread recvThread ;//to recv the message from clients.
	std::thread pullReplyThread ; //try to draw a reply from the queue
	messageQueue *mq ; 
	struct sockaddr_in address ;  
	socklen_t addrlen  ; 
	struct epoll_event event ; 
	struct epoll_event events[maxEvents]; 
public:
	linuxSockTcp() ;
	int threadInit() ; 
	int initServer();
	int recvConstantly();
	int pullReplyConstantly(); 	
	int sendReply(SerReply* serReply);
	UserBhr* jsonToUserBhr(json js,int userSocket);
	static int Send4Res(std::string data, int cliSock);
	static int Send(std::string data, int cliSock) ; 
};
#endif
