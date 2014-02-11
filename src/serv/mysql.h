#pragma once

#include <string>

#include <mysql_driver.h>
#include <mysql_connection.h>

#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>

namespace teh
{
	class MySQL
	{
		public:
			MySQL(const std::string& hostname, unsigned short int port, const std::string& username, const std::string& password, const std::string& schema);
			~MySQL();
		
			bool register_user(const std::string& username, const std::string& password, const unsigned short int& permissions);
			bool validate_login(const std::string& username, const std::string& password);
			unsigned short int get_permissions(const std::string& username);
			bool set_permissions(const std::string& username, const unsigned short int& permissions);
		
			sql::Connection* connect();
		private:
			std::string hash_password(std::string password);
			
			std::string _connectstr;
			std::string _username;
			std::string _password;
			std::string _schema;
		
			sql::mysql::MySQL_Driver* _driver;
	};
}
