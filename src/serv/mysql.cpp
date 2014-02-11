#include "mysql.h"

#include <sha.h>
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
	
	bool MySQL::register_user(const std::string& username, const std::string& password, const unsigned short int& permissions)
	{
		sql::Connection* conn = connect();
		
		std::string hashed = hash_password(password);
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("INSERT INTO `Users` VALUES (NULL, ?, ?, ?)");
		prep_stmt->setString(1, username.c_str());
		prep_stmt->setString(2, hashed.c_str());
		prep_stmt->setInt(3, permissions);
		int changed = prep_stmt->executeUpdate();
		delete prep_stmt;
		delete conn;
		return changed == 1;
	}
	
	bool MySQL::validate_login(const std::string& username, const std::string& password)
	{
		sql::Connection* conn = connect();
		
		std::string hashed = hash_password(password);
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `id` FROM `Users` WHERE `username` = ? AND `hashedpasswd` = ?");
		prep_stmt->setString(1, username.c_str());
		prep_stmt->setString(2, hashed.c_str());
		sql::ResultSet* res = prep_stmt->executeQuery();
		bool result = res->rowsCount() == 1;
		
		delete res;
		delete prep_stmt;
		delete conn;
		
		return result;
	}
	
	unsigned short int MySQL::get_permissions(const std::string& username)
	{
		sql::Connection* conn = connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `permissions` FROM `Users` WHERE `username` = ?");
		prep_stmt->setString(1, username.c_str());
		
		sql::ResultSet* res = prep_stmt->executeQuery();
		
		res->next();
		
		unsigned short int result = res->getUInt(1);
		
		delete conn;
		delete prep_stmt;
		delete res;
		
		return result;
	}
	
	bool MySQL::set_permissions(const std::string& username, const unsigned short int& permissions)
	{
		sql::Connection* conn = connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("UPDATE `Users` SET `permissions` = ? WHERE `username` = ?");
		prep_stmt->setUInt(1, permissions);
		prep_stmt->setString(2, username.c_str());
		
		bool result = false;
		if (prep_stmt->executeUpdate() == 1)
		{
			result = true;
		}
		
		delete conn;
		delete prep_stmt;
		
		return result;
	}
	
	std::string MySQL::hash_password(std::string password)
	{
		CryptoPP::SHA512 sha;
		unsigned char* buffer = new unsigned char[sha.DigestSize()];
		unsigned char* passwordbytes = new unsigned char[password.size()];
		memcpy(passwordbytes, password.c_str(), password.size());
		sha.CalculateDigest(buffer, passwordbytes, password.size());
		char* hashedbytes = new char[sha.DigestSize()];
		memcpy(hashedbytes, buffer, sha.DigestSize());
		std::string hashed(hashedbytes, sha.DigestSize());
		return hashed;
	}
	
	sql::Connection* MySQL::connect()
	{
		sql::Connection* conn = _driver->connect(_connectstr, _username, _password);
		conn->setSchema(_schema);
		return conn;
	}
}
