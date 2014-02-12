#include "rpgroom.h"
#include "rpggame.h"
#include "rpgcharacter.h"
#include "mysql.h"

namespace teh
{
	RPGRoom::RPGRoom(int id, RPGGame* parent)
		: _id(id), _parent(parent)
	{
		sql::Connection* conn = _parent->sql()->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `description` FROM `Rooms` WHERE `id` = ?");
		prep_stmt->setInt(1, _id);
		sql::ResultSet* res = prep_stmt->executeQuery();
		res->next();
		_description = res->getString("description");
		delete res;
		delete prep_stmt;
		delete conn;
		
		locate();
	}
	
	std::vector<RPGCharacter*> RPGRoom::get_occupants()
	{
		std::vector<RPGCharacter*> occupants;
		
		sql::Connection* conn = _parent->sql()->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `Characters`.`id` FROM `Rooms` JOIN `Characters` WHERE `Characters`.`room_id` = `Rooms`.`id` AND `Rooms`.`id` = ?");
		prep_stmt->setInt(1, _id);
		sql::ResultSet* res = prep_stmt->executeQuery();
		while (res->next())
		{
			RPGCharacter* character = _parent->get_character(res->getInt(1));
			occupants.push_back(character);
		}
		delete res;
		delete prep_stmt;
		delete conn;
		
		return occupants;
	}
	
	void RPGRoom::broadcast(const std::string& msg)
	{
		std::vector<RPGCharacter*> occupants = get_occupants();
		for (unsigned int n = 0;n < occupants.size();n++)
		{
			clientid client = _parent->check_logged_in(occupants[n]);
			if (client != -1)
			{
				_parent->message_client(client, msg);
			}
		}
	}
	
	void RPGRoom::locate()
	{
		sql::Connection* conn = _parent->sql()->connect();
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `xpos`, `ypos`, `zpos` FROM `Rooms` WHERE `id` = ?");
		prep_stmt->setInt(1, _id);
		sql::ResultSet* res = prep_stmt->executeQuery();
		res->next();
		
		_parent->locate_room(res->getInt("xpos"),res->getInt("ypos"),res->getInt("zpos"), this);
		
		delete res;
		delete prep_stmt;
		delete conn;
	}
}