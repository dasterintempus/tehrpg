#include "userutil.h"
#include "mysql.h"

namespace teh
{
	bool register_user(MySQL* sql, const std::string& username, const std::string& password, const unsigned short int& permissions)
	{
		sql::Connection* conn = sql->connect();
		
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
	
	bool validate_login(MySQL* sql, unsigned int userid, const std::string& challenge, const std::string& challengeresponse)
	{
		sql::Connection* conn = sql->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `hashedpasswd` FROM `Users` WHERE `id` = ?");
		prep_stmt->setUInt(1, userid);
		sql::ResultSet* res = prep_stmt->executeQuery();
		if (res->rowsCount() == 0)
		{
			delete res;
			delete prep_stmt;
			delete conn;
			return false;
		}
		
		res->next();
		
		std::string hashedpasswd = res->getString("hashedpasswd");
		delete res;
		delete prep_stmt;
		delete conn;
		
		std::string hexpasswd = to_hex(hashedpasswd);
		//std::cerr << "Hex passwd: " << hexpasswd << std::endl;
		
		std::string goodresponse = hash_sha512(challenge + hexpasswd);
		
		std::string hexgoodresponse = to_hex(goodresponse);
		
		return challengeresponse == hexgoodresponse;
	}
	
	unsigned int get_userid(MySQL* sql, const std::string& username)
	{
		sql::Connection* conn = sql->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `id` FROM `Users` WHERE `username` = ?");
		prep_stmt->setString(1, username.c_str());
		sql::ResultSet* res = prep_stmt->executeQuery();
		if (res->rowsCount() == 0)
		{
			delete res;
			delete prep_stmt;
			delete conn;
			return -1;
		}
		
		res->next();
		
		unsigned int id = res->getUInt(1);
		delete res;
		delete prep_stmt;
		delete conn;
		
		return id;
	}
	
	std::string get_username(MySQL* sql, unsigned int userid)
	{
		sql::Connection* conn = sql->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `id` FROM `Users` WHERE `id` = ?");
		prep_stmt->setUInt(1, userid);
		sql::ResultSet* res = prep_stmt->executeQuery();
		if (res->rowsCount() == 0)
		{
			delete res;
			delete prep_stmt;
			delete conn;
			return "";
		}
		
		res->next();
		
		std::string username = res->getString(1);
		delete res;
		delete prep_stmt;
		delete conn;
		
		return username;
	}
	
	unsigned short int get_permissions(MySQL* sql, unsigned int userid)
	{
		sql::Connection* conn = sql->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `permissions` FROM `Users` WHERE `id` = ?");
		prep_stmt->setUInt(1, userid);
		
		sql::ResultSet* res = prep_stmt->executeQuery();
		
		res->next();
		
		unsigned short int result = res->getUInt(1);
		
		std::cerr << result << std::endl;
		
		delete conn;
		delete prep_stmt;
		delete res;
		
		return result;
	}
	
	bool set_permissions(MySQL* sql, unsigned int userid, const unsigned short int& permissions)
	{
		sql::Connection* conn = sql->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("UPDATE `Users` SET `permissions` = ? WHERE `id` = ?");
		prep_stmt->setUInt(1, permissions);
		prep_stmt->setUInt(2, userid);
		
		bool result = false;
		if (prep_stmt->executeUpdate() == 1)
		{
			result = true;
		}
		
		delete conn;
		delete prep_stmt;
		
		return result;
	}
}