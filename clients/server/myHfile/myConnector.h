#pragma once

//#ifndef MYSQLCONNECTOR_H
//#define MYSQLCONNECTOR_H
#ifndef fknfeefeosdhjisfhiosd_H
#define fknfeefeosdhjisfhiosd_H
using namespace std;
//#include "mysqlConnector.h"
#include "jdbc/cppconn/prepared_statement.h"
#include "jdbc/cppconn/statement.h"
#include "jdbc/cppconn/resultset.h"
#include "jdbc/cppconn/exception.h"
#include "jdbc/mysql_driver.h" 
#include <string>
class mysqlInteraction {
public:
	mysqlInteraction(){}
	static int initMysql(sql::mysql::MySQL_Driver** driver,sql::Connection** con); 
	static int isUserNameExist(string user_name, sql::Connection* con); 
	static int matchNameAndPwd(string user_name, string user_pwd, sql::Connection* con); 
	static int insertAcInfo(string user_name, string user_pwd, sql::Connection* con); 
	static string findStringBy(string targetCol, string keyCol, string keyVal, sql::Connection* con); 
	static string findUserNameByID(int user_id, sql::Connection* con); 
	static int findIDByUserName(string user_name, sql::Connection* con); 
	static int findUserdqCoinByID(int user_id, sql::Connection* con); 
	static int setInt(string targetCol, int val, int user_id, sql::Connection* con);
};
#endif //  
