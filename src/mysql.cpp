#include "mysql.h"

#include <sha.h>
#include <iostream>
#include <sstream>
#include <iomanip>

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
		
		std::string hashed = hash_sha512(password);
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("INSERT INTO `Users` VALUES (NULL, ?, ?, ?)");
		prep_stmt->setString(1, username.c_str());
		prep_stmt->setString(2, hashed.c_str());
		prep_stmt->setInt(3, permissions);
		int changed;
		try
		{
			changed = prep_stmt->executeUpdate();
		}
		catch (sql::SQLException& e)
		{
			delete prep_stmt;
			delete conn;
			return false;
		}
		delete prep_stmt;
		delete conn;
		return changed == 1;
	}
	
	bool MySQL::validate_login(const std::string& username, const std::string& challenge, const std::string& challengeresponse)
	{
		sql::Connection* conn = connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `hashedpasswd` FROM `Users` WHERE `username` = ?");
		prep_stmt->setString(1, username.c_str());
		sql::ResultSet* res = prep_stmt->executeQuery();
		res->next();
		std::string hashedpasswd = res->getString("hashedpasswd");
		delete res;
		delete prep_stmt;
		delete conn;
		
		std::string hexpasswd = to_hex(hashedpasswd);
		std::cerr << "Hex passwd: " << hexpasswd << std::endl;
		
		std::string goodresponse = hash_sha512(challenge + hexpasswd);
		
		std::string hexgoodresponse = to_hex(goodresponse);
		
		return challengeresponse == hexgoodresponse;
	}
	
	unsigned short int MySQL::get_permissions(const std::string& username)
	{
		sql::Connection* conn = connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `permissions` FROM `Users` WHERE `username` = ?");
		prep_stmt->setString(1, username.c_str());
		
		sql::ResultSet* res = prep_stmt->executeQuery();
		
		res->next();
		
		unsigned short int result = res->getUInt(1);
		
		std::cerr << result << std::endl;
		
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
	
	sql::Connection* MySQL::connect()
	{
		sql::Connection* conn = _driver->connect(_connectstr, _username, _password);
		conn->setSchema(_schema);
		return conn;
	}
	
	std::string MySQL::hash_sha512(std::string input)
	{
		CryptoPP::SHA512 sha;
		unsigned char* buffer = new unsigned char[sha.DigestSize()];
		unsigned char* inputbytes = new unsigned char[input.size()];
		memcpy(inputbytes, input.c_str(), input.size());
		sha.CalculateDigest(buffer, inputbytes, input.size());
		char* hashedbytes = new char[sha.DigestSize()];
		memcpy(hashedbytes, buffer, sha.DigestSize());
		std::string hashed(hashedbytes, sha.DigestSize());
		delete[] buffer;
		delete[] inputbytes;
		delete[] hashedbytes;
		return hashed;
	}
	
	std::string MySQL::to_hex(std::string input)
	{
		std::stringstream conv;
		
		conv << std::hex << std::setfill('0');
		
		for (std::size_t i = 0; i != input.size(); ++i)
		{
			unsigned int current_byte_number = static_cast<unsigned int>(static_cast<unsigned char>(input[i]));
			conv << std::setw(2) << current_byte_number;
		}
		
		return conv.str();
		
		/*
		
		char* str_buf = new char[2*input.size() + 1];

		for (unsigned int i = 0; i < input.size(); i++)
		{
		    snprintf(str_buf, 2, "%02X", input[i]);  // need 2 characters for a single hex value
		}
		snprintf(str_buf, 1, ""); // dont forget the NULL byte
		std::string hexed(str_buf);
		std::cerr << "hex: " << hexed << std::endl;
		delete str_buf;
		return hexed;
		*/
	}
}
