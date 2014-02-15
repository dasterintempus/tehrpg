#include "rpgcharacter.h"
#include "rpggame.h"
#include "rpgroom.h"
#include "mysql.h"

namespace teh
{
	RPGCharacter::RPGCharacter(int id, RPGGame* parent)
		: _id(id), _parent(parent)
	{
		sql::Connection* conn = _parent->sql()->connect();
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT * FROM `Characters` WHERE `id` = ?");
		prep_stmt->setInt(1, _id);
		sql::ResultSet* res = prep_stmt->executeQuery();
		res->next();
		
		_name = res->getString("name");
		_strength = res->getUInt("strength");
		_constitution = res->getUInt("constitution");
		_dexterity = res->getUInt("dexterity");
		_intelligence = res->getUInt("intelligence");
		_wisdom = res->getUInt("wisdom");
		_charisma = res->getUInt("charisma");
		
		delete res;
		delete prep_stmt;
		delete conn;
	}
	
	unsigned short int RPGCharacter::strength()
	{
		return _strength;
	}
	
	unsigned short int RPGCharacter::constitution()
	{
		return _constitution;
	}
	
	unsigned short int RPGCharacter::dexterity()
	{
		return _dexterity;
	}
	
	unsigned short int RPGCharacter::intelligence()
	{
		return _intelligence;
	}
	
	unsigned short int RPGCharacter::wisdom()
	{
		return _wisdom;
	}
	
	unsigned short int RPGCharacter::charisma()
	{
		return _charisma;
	}

	std::string RPGCharacter::name()
	{
		return _name;
	}
	
	RPGRoom* RPGCharacter::get_location()
	{
		sql::Connection* conn = _parent->sql()->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `Rooms`.`id` FROM `Characters` JOIN `Rooms` WHERE `Characters`.`room_id` = `Rooms`.`id` AND `Characters`.`id` = ?");
		prep_stmt->setInt(1, _id);
		sql::ResultSet* res = prep_stmt->executeQuery();
		res->next();
		
		RPGRoom* room = _parent->get_room(res->getInt(1));
		
		delete res;
		delete prep_stmt;
		delete conn;
		
		return room;
	}
	
	void RPGCharacter::say(const std::string& msg)
	{
		get_location()->broadcast(_name + " says: " + msg);
	}
}