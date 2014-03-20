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
		
			sql::Connection* connect();
		private:
			std::string _connectstr;
			std::string _username;
			std::string _password;
			std::string _schema;
		
			sql::mysql::MySQL_Driver* _driver;
	};
}
