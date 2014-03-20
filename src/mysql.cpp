#include "mysql.h"
#include <sstream>

namespace teh
{
	MySQL::MySQL(const std::string& hostname, unsigned short int port, const std::string& username, const std::string& password, const std::string& schema)
		: _username(username), _password(password), _schema(schema),
		_driver(sql::mysql::get_mysql_driver_instance())
	{
		std::stringstream connectstrstream;
		connectstrstream << "tcp://" << hostname << ":" << port;
		_connectstr = connectstrstream.str();
	}
	
	MySQL::~MySQL()
	{
		
	}
	
	sql::Connection* MySQL::connect()
	{
		sql::Connection* conn = _driver->connect(_connectstr, _username, _password);
		conn->setSchema(_schema);
		return conn;
	}
}
