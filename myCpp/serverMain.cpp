#include <stdio.h>
#include "jdbc/cppconn/prepared_statement.h"
#include "jdbc/cppconn/statement.h" 
#include "jdbc/cppconn/resultset.h" 
#include "jdbc/cppconn/exception.h"
#include "jdbc/mysql_driver.h"
#include <string>
#include "linuxSockTcp.h"
#include "HallInterface.h"
#include "myConnector.h"
#include "HallBC.h"
#include "GameBC.h"
using namespace std;
int main(){
	sql::mysql::MySQL_Driver* driver = nullptr;     
	sql::Connection* con = nullptr;     
	sql::Statement* stmt = nullptr ;     
	sql::PreparedStatement* pstmt = nullptr;     
	sql::ResultSet* res = nullptr;
	mysqlInteraction::initMysql(&driver,&con);	
	std::cout <<"root:" << mysqlInteraction::isUserNameExist("root",con) << std::endl ; 
	string creTable = "create table if not exists users( "         
		"user_id INT PRIMARY KEY NOT NULL AUTO_INCREMENT ,"        
	       	"user_name VARCHAR(64) NOT NULL ,"        
	       	"user_pwd VARCHAR(64)NOT NULL ,"        
	       	"user_dqCoin INT DEFAULT 0 ,"        
	       	"user_state INT DEFAULT -1  ,"         
		"root_id INT DEFAULT -1); ";
	stmt = con->createStatement() ; 
	try{
		stmt->execute(creTable); 
	}
	catch(sql::SQLException &e){;}	
	Hall* hall = new Hall(con);
	replyQueue* rq = new replyQueue();
	messageQueue* mq = new messageQueue(hall,rq);
	linuxSockTcp *lst = new linuxSockTcp(mq);
	HallBC *hallbc = new HallBC(hall,rq); 
	GameBC* gamebc = new GameBC(hall,rq); 
	while(1); 
	return 0 ; 
}

