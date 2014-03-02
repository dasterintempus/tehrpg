#pragma once

#include <mysql.h>

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
			bool validate_login(unsigned int userid, const std::string& challenge, const std::string& challengeresponse);
			unsigned int get_userid(const std::string& username);
			std::string get_username(unsigned int userid);
			unsigned short int get_permissions(unsigned int userid);
			bool set_permissions(unsigned int userid, const unsigned short int& permissions);
		
			sql::Connection* connect();
		private:
			std::string hash_sha512(std::string input);
			std::string to_hex(std::string input);
			
			std::string _connectstr;
			std::string _username;
			std::string _password;
			std::string _schema;
		
			sql::mysql::MySQL_Driver* _driver;
	};
}
