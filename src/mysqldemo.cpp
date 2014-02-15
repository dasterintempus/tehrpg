#include <iostream>

#include <mysql_driver.h>
#include <mysql_connection.h>

#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>

void mysqldemo()
{
	sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
	sql::Connection* con = driver->connect("tcp://127.0.0.1:3306", "bobfpcomm", "maxcom01");
	
	con->setSchema("bobfpcomm");
	
	sql::PreparedStatement* prep_stmt = con->prepareStatement("SELECT `value` FROM `dummy` WHERE id = ?");
	
	prep_stmt->setInt(1,1);
	sql::ResultSet* res = prep_stmt->executeQuery();
	
	
	while (res->next())
	{
		std::cout << res->getString("value") << std::endl;
	}
	
	delete res;
	delete prep_stmt;
	delete con;
}
