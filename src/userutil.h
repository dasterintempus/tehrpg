#pragma once

#include "mysql.h"
#include "stringutil.h"

namespace teh
{			
	bool register_user(MySQL* sql, const std::string& username, const std::string& password, const unsigned short int& permissions);
	bool validate_login(MySQL* sql, unsigned int userid, const std::string& challenge, const std::string& challengeresponse);
	unsigned int get_userid(MySQL* sql, const std::string& username);
	std::string get_username(MySQL* sql, unsigned int userid);
	unsigned short int get_permissions(MySQL* sql, unsigned int userid);
	bool set_permissions(MySQL* sql, unsigned int userid, const unsigned short int& permissions);
}