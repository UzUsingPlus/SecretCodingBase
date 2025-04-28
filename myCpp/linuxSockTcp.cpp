#include "linuxSockTcp.h"
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <string.h>
#include <string>
#include <thread>
using namespace std ; 
const int MAX_BUFFER_SIZE = 2048;
linuxSockTcp::linuxSockTcp(){
	endMark = 0 ; 
	onlineNum = 0 ;	
	this->initServer();
	this->threadInit();
}
linuxSockTcp::linuxSockTcp(messageQueue* mq)
:mq(mq)
{
	endMark = 0 ; 
	onlineNum = 0 ;	
	this->initServer();
	this->threadInit();
}
int linuxSockTcp::threadInit(){  
	this->recvThread = std::thread(&linuxSockTcp::recvConstantly,this);
	this->pullReplyThread = std::thread(&linuxSockTcp::pullReplyConstantly,this);//to finish       
	return 1 ; 
}
int linuxSockTcp::initServer() {
	this->addrlen = sizeof(address);
	char buf[MAX_BUFFER_SIZE] = { 0 };
	this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->server_fd == 0) {
		std::cout << "SOCKET CREATE ERROR " << std::endl;
		return -1;
	}
	this->address.sin_family = AF_INET;
	this->address.sin_addr.s_addr = INADDR_ANY;
	this->address.sin_port = htons(9999);
		if (bind(this->server_fd, (struct sockaddr*)(&address), addrlen) < 0) {
			std::cout << "Binding failed " << std::endl; 
			return -1;
		}
	if(listen(this->server_fd,3) < 0 ){//let the server listen constantlt 
		std::cout << "Listening failed "<<std::endl;  
		return -1;  
	}
		this->epoll_fd = epoll_create1(0);
		if(this->epoll_fd == -1){
			std::cout << "Epoll Creating failed" << std::endl ; 
		return -1 ;    
}	       
	this->event.events = EPOLLIN ; 
	this->event.data.fd = this->server_fd ; 
	if(epoll_ctl(this->epoll_fd,EPOLL_CTL_ADD,server_fd,&(this->event))==-1){
		std::cout << "Epoll controlling failed "<<std::endl ; 
	return -1 ; 
}
	std::cout << "The server was initialized successfully " << std::endl ; 
	return 1;
}
int linuxSockTcp::recvConstantly(){
	int client_fd ; 
	std::cout << "Receiving ... ... " << std::endl ; 
	while(!(this->endMark)){
		int nfds = epoll_wait(this->epoll_fd,this->events,maxEvents,-1) ; //num of interested fd
		for(int i = 0 ; i < nfds ; ++i){
			if(this->events[i].data.fd == this->server_fd){
				client_fd = accept(this->server_fd,(struct sockaddr*)(&(this->address)),&addrlen) ;	
				this->event.events =EPOLLIN ; 
				this->event.data.fd = client_fd ; 
				if(epoll_ctl(this->epoll_fd,EPOLL_CTL_ADD,client_fd,&event)==-1){
					continue ; 
			}
			std::cout << "Accpted a new connection " << client_fd << std::endl ; 	
			}else{
				char buffer[MAX_BUFFER_SIZE] = {0} ; 
			    int bytes_read = read(this->events[i].data.fd,buffer,sizeof(buffer));
				if(bytes_read <=0){
					std::cout << "Close Connection :" << events[i].data.fd << std::endl ; 
					close(events[i].data.fd);	
				}else{
					std::cout <<"Recv: "<< buffer << std::endl ; 
					string s(buffer) ; 
					try{
						json js = json::parse(s);
						this->mq->pushUserBhr(this->jsonToUserBhr(js,events[i].data.fd));
					}
					catch(...){

					}
				}
			}
				
		}
			
	}
	return 1; 
}
UserBhr* linuxSockTcp::jsonToUserBhr(json js,int userSocket){
	UserBhr *userBhr = nullptr ;
	int stage = js["userStage"]; 
	switch (stage/100){
	   case 0:{
			userBhr = new RegisBhr();
			userBhr->analyzeJsonToStruct(js);
		    break; 
	   }case 1:{
			userBhr = new HallBhr();
			userBhr->analyzeJsonToStruct(js);
		  break; 
	   }case 2:{
			userBhr = new GameBhr();
			userBhr->analyzeJsonToStruct(js);
		  	break;
	   }
	   default: 
			return 0 ; 
	}
	//userBhr->analyzeJsonToStruct(js); 
	userBhr->userSocket = userSocket ;
	return userBhr ;  
}
int linuxSockTcp::pullReplyConstantly(){
	while(!(this->endMark)){
		SerReply* serReply	= this->mq->replyQueuePop(); 
		if(serReply){//not nullptr sending 
			this->sendReply(serReply);
		}
		else//is nullptr sleep zzz..
		 	std::this_thread::sleep_for(std::chrono::milliseconds(500)); 
	}
	return 1 ; 
}
int linuxSockTcp::sendReply(SerReply* serReply){
	if( serReply->userSocket == -1 ){
		delete serReply ; 
		return -1 ; 
	}
	std::string data ; //maybe json..? i dont know
	int userState = serReply->userStage/100 ; 
	json js ; 
	if(userState == 0 ){
		RegisReply* rr = dynamic_cast<RegisReply*>(serReply);
		js = rr->analyzeStructToJson(); 
	}else if(userState == 1){
		HallReply* hr = dynamic_cast<HallReply*>(serReply);
		js = hr->analyzeStructToJson();
	}else if(userState == 2){
		GameReply* gr = dynamic_cast<GameReply*>(serReply);
		js = gr->analyzeStructToJson();
	}else if(userState == 3){
		HallBroadCastReply* hbcr = dynamic_cast<HallBroadCastReply*>(serReply);
		js = hbcr->analyzeStructToJson();
	}
	data = js.dump();
	cout << "dataStr:" << data << endl;  
	//how to turn json into data..
	int tryCnt = 10 ;
	
	while(!linuxSockTcp::Send(data,serReply->userSocket)&&tryCnt){
		std::this_thread::sleep_for(std::chrono::milliseconds(this->errGap));
		tryCnt-- ; 
	}
	delete serReply ;
	return tryCnt ? 1 : 0 ;
}

int linuxSockTcp::Send4Res(string data, int cliSock) {
        char buf[MAX_BUFFER_SIZE] = { 0 };
        strcpy(buf, data.c_str());
        if (!send(cliSock, (void*)buf, sizeof(buf), 0))
            return -1;
        memset(buf, 0, sizeof(buf));
        if (!recv(cliSock, (void*)buf, sizeof(buf), 0))
            return 0;
        return atoi(buf);
    }
int linuxSockTcp::Send(string data, int cliSock) {
        char buf[MAX_BUFFER_SIZE] = { 0 };
        strcpy(buf, data.c_str());
		uint32_t len = data.size();
		uint32_t net_len = htonl(len);
		send(cliSock,&net_len,4,0);//send the date length
		send(cliSock,(void*)buf,len,0);//send the fucking data..
        return 1;
}
