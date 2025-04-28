/* #include "jdbc/cppconn/prepared_statement.h"
#include "jdbc/cppconn/statement.h"
#include "jdbc/cppconn/resultset.h"
#include "jdbc/cppconn/exception.h"
#include "jdbc/mysql_driver.h" 
#include <string> */
#include "myConnector.h"
using namespace std; 
int mysqlInteraction::initMysql(sql::mysql::MySQL_Driver** driver,sql::Connection** con){
	 (*driver) = sql::mysql::get_mysql_driver_instance();
	 std::cout << "Attempting to connect..." << std::endl;        
	 (*con) = (*driver)->connect("tcp://localhost:3306", "root", "root");
	 if(!(*con)){ 
		std::cout <<"Connecting failed."<<std::endl;  
		 return -1 ;
	 } 
	 std::cout << "Connecting successfully!" << std::endl;        
	 try{
	 	(*con)->setSchema("game_database") ;
	 }catch(sql::SQLException &e){
         	string creDatabase = "create database game_database" ; 
		sql::Statement* stmt = (*con)->createStatement() ; 
		stmt->execute(creDatabase); 
		(*con)->setSchema("game_database");
	        free(stmt) ; 	
	 }		 
	 return 1 ; 
}
int mysqlInteraction::isUserNameExist(string user_name, sql::Connection* con) {
        if(!con)
	    return -1 ; 
	try {
            sql::ResultSet* res;
            string query = "select * from users where user_name=?;";
            sql::PreparedStatement* pstmt = con->prepareStatement(query);
            pstmt->setString(1, user_name);
	    res = pstmt->executeQuery();
	    return res->next() ? 1 : 0;
        }
        catch (sql::SQLException& e) {
            std::cerr << e.what() << std::endl;
            return -1;
        }
    }
int mysqlInteraction::matchNameAndPwd(string user_name, string user_pwd, sql::Connection* con) {
        try {
            sql::ResultSet* res;
            string query = "select user_pwd from users where user_name=?;";
            sql::PreparedStatement* pstmt = con->prepareStatement(query);
            pstmt->setString(1, user_name);
            res = pstmt->executeQuery();
            return res->next() && res->getString("user_pwd") == user_pwd ? 1 : 0;
        }
        catch (sql::SQLException& e) {
            std::cerr << e.what() << std::endl;
            return -1;
        }
    }
int mysqlInteraction::insertAcInfo(string user_name, string user_pwd, sql::Connection* con) {
       if (mysqlInteraction::isUserNameExist(user_name, con)) return 0;
        sql::PreparedStatement* pstmt = con->prepareStatement("insert into users(user_name,user_pwd) VALUES(?,?);");
        try {
            pstmt->setString(1, user_name);
            pstmt->setString(2, user_pwd);
            pstmt->execute();
        }
        catch (sql::SQLException& e) {
            std::cerr << e.what() << endl;
            pstmt->close();
            delete pstmt;
            return -1;
        }
        pstmt->close();
        delete pstmt;
        return 1;
    }
string mysqlInteraction::findStringBy(string targetCol, string keyCol, string keyVal, sql::Connection* con) {
        try {
            sql::ResultSet* res;
            string query = "select * from users where " + keyCol + "=?;";
            sql::PreparedStatement* pstmt = con->prepareStatement(query);
            pstmt->setString(1, keyVal);
            res = pstmt->executeQuery();
            return res->next() ? res->getString(targetCol) : "NotFound";
        }
        catch (sql::SQLException& e) {
            std::cerr << e.what() << endl;
            return "";        }
    }
string mysqlInteraction::findUserNameByID(int user_id, sql::Connection* con) {
        try {
            sql::ResultSet* res;
            string query = "select user_name from users where user_id =?;";
            sql::PreparedStatement* pstmt = con->prepareStatement(query);
            pstmt->setInt(1, user_id);
            res = pstmt->executeQuery();
            return res->next() ? res->getString("user_name") : "NotFound";
        }
        catch (sql::SQLException& e) {
            std::cerr << e.what() << endl;
            return "";
        }
    }
int mysqlInteraction::findIDByUserName(string user_name, sql::Connection* con) {
        try {
            sql::ResultSet* res;
            string query = "select user_id from users where user_name =?;";
            sql::PreparedStatement* pstmt = con->prepareStatement(query);
            pstmt->setString(1, user_name);
            res = pstmt->executeQuery();
            return res->next() ? res->getInt("user_id") : -1;
        }
        catch (sql::SQLException& e) {
            std::cerr << e.what() << endl;
            return 0;
        }
    }
int mysqlInteraction::findUserdqCoinByID(int user_id, sql::Connection* con) {
        try {
            sql::ResultSet* res;
            string query = "select user_dqCoin from users where user_id = ?;";
            sql::PreparedStatement* pstmt = con->prepareStatement(query);
            pstmt->setInt(1, user_id);
            res = pstmt->executeQuery();
            return res->next() ? res->getInt("user_dqCoin") : -1;
        }
        catch (sql::SQLException& e) {
            std::cerr << e.what() << endl;
            return -1;
        }
    }
int mysqlInteraction::setInt(string targetCol, int val, int user_id, sql::Connection* con) {
        try {
            string query = "update users set " + targetCol + " =? where user_id = ? ; ";
            sql::PreparedStatement* pstmt = con->prepareStatement(query);
            pstmt->setInt(1, val);
            pstmt->setInt(2, user_id);
            pstmt->execute();
            pstmt->close();
            delete pstmt;
            return 1;
        }
        catch (sql::SQLException& e) {
            std::cerr << e.what() << std::endl;
            return -1;
        }
        return 0;
    }

/*
int main(){
std::string sqlCre = "create database game_database";        

sql::mysql::MySQL_Driver* driver = nullptr;    
sql::Connection* con = nullptr;    
sql::Statement* stmt = nullptr ;    
sql::PreparedStatement* pstmt = nullptr;    
sql::ResultSet* res = nullptr;        
driver = sql::mysql::get_mysql_driver_instance();  
std::cout << "Attempting to connect..." << std::endl;  
con = driver->connect("tcp://localhost:3306", "root", "root");   
std::cout << "Connected successfully!" << std::endl;   
con->setSchema("game_database") ; 
stmt = con->createStatement(); 
 stmt->execute(sqlCre);
con->setSchema("game_database");
std::string tableCre = "create table if not exists users("
"user_id INT NOT NULL primary key AUTO_INCREMENT,  "
"user_name VARCHAR ( 31 ) NOT NULL,  "
"user_pwd VARCHAR ( 31 ) NOT NULL,  "
"user_dqCoin INT DEFAULT 0,  "
"user_state INT DEFAULT 0,  "
"room_id INT DEFAULT -1 );" ;  
stmt = con->createStatement(); 
stmt->execute(tableCre) ; 
std::cout << "create table success! " << std::endl ; 

stmt->execute("insert into users(user_name,user_pwd) values('root','root');"); 

}
*/
