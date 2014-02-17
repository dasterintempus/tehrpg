#include "rpgcharacter.h"
#include "rpggame.h"
#include "rpgroom.h"
#include "mysql.h"
#include <sstream>

namespace teh
{
	const std::string RPGCharacter::StatNames[6] = {"strength", "constitution", "dexterity", "intelligence", "wisdom", "charisma"};
	
	RPGCharacter* RPGCharacter::build(RPGGame* parent, const std::string& name, const std::string& username, RPGRoom* room, const std::map<std::string, unsigned short int>& stats)
	{
		for (unsigned int n=0;n < 6;n++)
		{
			if (stats.count(StatNames[n]) == 0)
			{
				return 0;
			}
		}
		
		sql::Connection* conn = parent->sql()->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("INSERT INTO `Characters` VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, (SELECT `id` FROM `Users` WHERE `username` = ? LIMIT 1), ?)");
		prep_stmt->setString(1, name);
		prep_stmt->setUInt(2, stats.at("strength"));
		prep_stmt->setUInt(3, stats.at("constitution"));
		prep_stmt->setUInt(4, stats.at("dexterity"));
		prep_stmt->setUInt(5, stats.at("intelligence"));
		prep_stmt->setUInt(6, stats.at("wisdom"));
		prep_stmt->setUInt(7, stats.at("charisma"));
		prep_stmt->setString(8, username);
		prep_stmt->setUInt(9, room->id());
		try
		{
			prep_stmt->execute();
		}
		catch (sql::SQLException &e)
		{
			delete prep_stmt;
			delete conn;
			return 0;
		}
		
		delete prep_stmt;
		
		prep_stmt = conn->prepareStatement("SELECT LAST_INSERT_ID()");
		sql::ResultSet* res = prep_stmt->executeQuery();
		res->next();
		unsigned int id = res->getUInt(1);
		delete res;
		delete prep_stmt;
		delete conn;
		
		return new RPGCharacter(id, parent);
	}
	
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
		prep_stmt->setInt(1, id());
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
		get_location()->broadcast(_name + " says: \"" + msg + "\"");
	}
	
	RPGRoom* RPGCharacter::move(const unsigned short int& axis, const short int& delta)
	{
		if (abs(delta) != 1)
			return 0;
		if (axis > 2)
			return 0;
		
		RPGRoom* location = get_location();
		long int xpos = location->xpos();
		long int ypos = location->ypos();
		long int zpos = location->ypos();
		if (axis == 0)
		{
			//x axis
			xpos += delta;
		}
		else if (axis == 1)
		{
			//y axis
			ypos += delta;
		}
		else if (axis == 2)
		{
			//z axis
			zpos += delta;
		}
		
		if (!RPGGame::valid_coord(xpos, ypos, zpos))
		{
			return 0;
		}
		
		if (!location->can_exit(xpos - location->xpos(), ypos - location->ypos(), zpos - location->zpos()))
		{
			return 0;
		}
		
		RPGRoom* destination = _parent->find_room(xpos, ypos, zpos);
		if (destination)
		{
			update_location(destination);
		}
		return destination;
	}
	
	std::string RPGCharacter::look()
	{
		std::stringstream sstream;
		RPGRoom* location = get_location();
		sstream << "You look around and see: " << location->description() << "\n";
		//get who else is here
		std::vector<RPGCharacter*> occupants = location->get_occupants();
		if (occupants.size() > 1)
		{
			sstream << "Here with you are: ";
		}
		for (unsigned int n=0;n<occupants.size();n++)
		{
			if (occupants[n] == this)
				continue;
			if (n == occupants.size()-1)
				sstream << occupants[n]->name() << "\n";
			else
				sstream << occupants[n]->name() << ", ";
		}
		//Get exits
		stringvector exits = location->get_exits();
		if (exits.size() > 0)
		{
			sstream << "There are exits heading: ";
		}
		for (unsigned int n=0;n<exits.size();n++)
		{
			if (n == occupants.size()-1)
				sstream << exits[n] << "\n";
			else
				sstream << exits[n] << ", ";
		}
		if (exits.size() > 0)
		{
			sstream << "\n";
		}
		return sstream.str();
	}
	
	unsigned int RPGCharacter::id()
	{
		return _id;
	}
	
	void RPGCharacter::update_location(RPGRoom* destination)
	{
		sql::Connection* conn = _parent->sql()->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("UPDATE `Characters` SET `room_id` = ? WHERE `id` = ?");
		prep_stmt->setInt(1, destination->id());
		prep_stmt->setInt(2, id());
		prep_stmt->execute();
		
		delete prep_stmt;
		delete conn;
	}
}