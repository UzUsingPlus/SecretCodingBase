#include "winSockTcp.h"
#include "WinSock2.h"
#pragma comment(lib,"ws2_32.lib")
const int MAX_BUFFER_SIZE = 2048 ;
winSockTcp::winSockTcp() {
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        std::cout << "WSA fail" << std::endl;
        return ;
    }
    this->serIP = "162.14.123.172";
    this->endMark = 0 ;
    getLocalIP();
    this->initConnection();
}
winSockTcp::winSockTcp(replyQueue* rq,messageQueue* mq):
    rq(rq),mq(mq)
{
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        std::cout << "WSA fail" << std::endl;
        return ;
    }
    this->serIP = "162.14.123.172";
    this->endMark = 0 ;
    getLocalIP();
    // the threadINit is in the initconnection()!!!!
    this->initConnection();
}
string winSockTcp::jsonToString(json js){
    return js.dump(); //todo
}
SerReply* winSockTcp::jsonToSerReply(json js){
    SerReply* serReply = nullptr ;//to do
    int userStage =js["userStage"];
    int userState = userStage/100 ;
    switch(userState){
    case 0:{//regis
        serReply = new RegisReply();
        serReply->analyzeJsonToStruct(js);
        break ;
    }case 1:{//hall
        serReply = new HallReply();
        serReply->analyzeJsonToStruct(js);
        break;
    }case 2:{//game
        serReply = new GameReply();
        serReply->analyzeJsonToStruct(js);
        break ;
    }case 3:{//hallBc
        serReply = new HallBroadCastReply();
        serReply->analyzeJsonToStruct(js);
    }
    default:{
        //fault serReply
        break ;
    }
    }
    return serReply ;
}
// above two is to do
int winSockTcp::initConnection() {
    int tryConnectcnt = 0;
    while (tryConnectcnt < 10) {
        for (int i = 0; i < this->localIP.size(); ++i) {
            this->cliSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);// create socket
            // unsigned long mode = 1;
            // ioctlsocket(this->cliSock,FIONBIO,&mode);
            if (this->cliSock == INVALID_SOCKET) {
                std::cout << " Creating serSocket failed" << std::endl;
                return -1;
            }
            this->sockAddr.sin_family = AF_INET;
            this->sockAddr.sin_port = htons(9999);
            this->sockAddr.sin_addr.S_un.S_addr = inet_addr(localIP[i].c_str());
            bind(this->cliSock, (struct sockaddr*)(&(this->sockAddr)), sizeof(SOCKADDR_IN));
            // if (bind(this->cliSock, (struct sockaddr*)(&(this->sockAddr)), sizeof(SOCKADDR_IN)) == 0 ) {
            //     std::cout << "Binding failed:" << WSAGetLastError() << std::endl;
            //     return -1;
            // }
            SOCKADDR_IN serAddr;
            serAddr.sin_family = AF_INET;
            serAddr.sin_port = htons(9999);
            serAddr.sin_addr.S_un.S_addr = inet_addr(serIP.c_str());
            std::cout << this->localIP[i] << std::endl;
            std::cout << this->serIP << std::endl;
            if (0==connect(this->cliSock, (SOCKADDR*)&serAddr, sizeof(SOCKADDR))) {
                tryConnectcnt = 100 ;
                break;
            }
            std::cout << "Connecting failed:" << WSAGetLastError() << std::endl;
            ++tryConnectcnt;
            Sleep(1000);
        }
    }
    if (tryConnectcnt == 10) {
        std::cout << "Can't connect to the server .Plz try it later . " << std::endl;
        return -1;
    }
    this->threadInit();
    std::cout << "Connection initialized successfully " << std::endl;
    return 1;
}
void winSockTcp::threadInit(){
    this->recvThread = std::thread(&winSockTcp::recvConstantly,this);
    this->sendThread = std::thread(&winSockTcp::sendConstantly,this);
}
void winSockTcp::getLocalIP() {
    ULONG outBufLen = sizeof(IP_ADAPTER_ADDRESSES) * 20; // 假设最多有 20 个适配器
    IP_ADAPTER_ADDRESSES* pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
    if (GetAdaptersAddresses(AF_INET, 0, nullptr, pAddresses, &outBufLen) == NO_ERROR) {
        for (IP_ADAPTER_ADDRESSES* pCurrAddresses = pAddresses; pCurrAddresses != nullptr; pCurrAddresses = pCurrAddresses->Next) {
            if (pCurrAddresses->OperStatus == IfOperStatusUp) {
                char ipBuffer[INET_ADDRSTRLEN];
                for (IP_ADAPTER_UNICAST_ADDRESS* pUnicast = pCurrAddresses->FirstUnicastAddress; pUnicast != nullptr; pUnicast = pUnicast->Next) {
                    SOCKADDR_IN* sa_in = (SOCKADDR_IN*)pUnicast->Address.lpSockaddr;
                    inet_ntop(AF_INET, &sa_in->sin_addr, ipBuffer, sizeof(ipBuffer));
                    this->localIP.push_back(ipBuffer);
                }
            }
        }
    }
    free(pAddresses);
}
void winSockTcp::shutdown(){
    closesocket(this->cliSock);
    WSACleanup();
}
int winSockTcp::Send4Res(string data) {
    if (send(this->cliSock, data.c_str(), data.size(), 0) < 0){
        cout << "sending fail" <<endl ;
        return -1;
    }
    char buf[MAX_BUFFER_SIZE] = { 0 };
    if (recv(this->cliSock, buf, 1024, 0)<0)
        return 0;
    return atoi(buf);
}
int winSockTcp::Send(string data) {
    if (send(this->cliSock , data.c_str(), data.size(), 0) < 0)
        return -1;
    return 1;
}
void winSockTcp::sendConstantly(){
    cout << "The sending thread init succ.." << endl ;
    while(!(this->endMark)){
        string info ;
        UserBhr *userBhr = this->mq->popUserBhr();
        if(userBhr){
            json js = this->UserBhrToJson(userBhr);
            string str = this->jsonToString(js);
            cout << "Sending data: " << str << endl ;
            this->Send(str);
        }else{
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
}
json winSockTcp::UserBhrToJson(UserBhr* userBhr){//to do
    json js ;
    int userState = userBhr->userStage/100 ;
    if(userState == 0 ){
        RegisBhr* rb = dynamic_cast<RegisBhr*>(userBhr);
        js = rb->struct_to_json();
    }else if(userState ==1){
        HallBhr* hb = dynamic_cast<HallBhr*>(userBhr) ;
        js = hb->struct_to_json();
    }else if(userState == 2){
        GameBhr* gb = dynamic_cast<GameBhr*>(userBhr);
        js = gb->struct_to_json();
    }
    return js ;
}
int winSockTcp::Recv(string& data) {
    int size = 0 ;
    char *buff = nullptr ;
    try{
        buff = (char*)malloc(sizeof(char)*MAX_BUFFER_SIZE);
        if( !buff ){
            cerr<< "the buff allotting failed.. fuck " <<endl ;
            return 1;
        }
        std::memset(buff,0,MAX_BUFFER_SIZE);
        //char buff[MAX_BUFFER_SIZE];
        size = recv(this->cliSock, buff , MAX_BUFFER_SIZE , 0);
        char *packet_end = (char*)memchr(buff,'\n',MAX_BUFFER_SIZE);
        size_t packet_length ;
        if(packet_end){
            packet_length  = packet_end - buff ;
            cout << "Packet_length:  "<<packet_length << endl ;
        }
        if (size < 0) {
            cout << "recv error" << endl;
            free(buff) ;
            return -1;
        }
        else if (size == 0) {
            cout << "peer shutdown connect " << endl;
            free(buff) ;
            return 0;
        }
        else {
            try{
                data.assign(buff, packet_length);
                free(buff) ;
            }catch(...){
                free(buff) ;
                cerr << "the data assign wrong" << endl ;
            }
        }
    }catch(...){
        cerr << "recv() break down " << endl ;
        free(buff);
    }
    return 1 ;

}
// char* nullPos = std::find(buff, buff + size, '\0');
// if (nullPos != buff + size) {
//     // 如果找到 '\0'，截断字符串
//     data.assign(buff, nullPos - buff);
// } else {
//     // 如果没有找到 '\0'，就直接使用接收到的数据
//     data.assign(buff, size);
// }
int winSockTcp::RecvInfo(string& data){
    char buff[MAX_BUFFER_SIZE] ={0} ;
    int size = recv(this->cliSock,buff,sizeof(buff),0);
    if(size <= 0 )
        data = "error";
    data.assign(buff,size);
    return 1 ;
}
void winSockTcp::recvConstantly(){
    char buff[MAX_BUFFER_SIZE];
    size_t buf_used = 0 ;
    while(!(this->endMark)){
        if( buf_used < 4){
            int need = 4-buf_used ;
            int recv_len = recv(this->cliSock,buff+buf_used,need,0);
            buf_used += recv_len ;
            if( buf_used < 4 )
                continue ;
        }
        uint32_t data_len = ntohl(*(uint32_t*)buff);
        while( buf_used - 4 < data_len ){
            int need = data_len - (buf_used-4) ;
            int recv_len = recv(this->cliSock,buff+buf_used,need,0);
            buf_used += recv_len ;
        }

        std::string data(buff+4,data_len);
        json js = json::parse(data);
        SerReply* serReply = this->jsonToSerReply(js);
        this->rq->pushSerReply(serReply) ;

        memmove(buff,buff+4+data_len,buf_used-4-data_len);
        buf_used -= 4 + data_len ;
    }
}



