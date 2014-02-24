#include "rpgcharacter.h"
#include "rpggame.h"
#include "rpgtile.h"
#include "rpginventory.h"
#include "rpgitemtype.h"
#include "rpgiteminstance.h"
#include "mysql.h"
#include <sstream>

namespace teh
{
namespace RPG
{
	const std::string Character::StatNames[6] = {"strength", "constitution", "dexterity", "intelligence", "wisdom", "charisma"};
	
	Character* Character::build(Game* parent, const std::string& name, const std::string& username, Tile* tile, const std::map<std::string, unsigned short int>& stats)
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
		prep_stmt->setUInt(9, tile->id());
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
		
		Character* character = parent->get_character(id);
		
		Inventory* inventory = character->add_inventory("backpack", 30);
		//inventory->fill_out();
		
		return character;
	}
	
	Character::Character(unsigned int id, Game* parent)
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
	
	unsigned short int Character::strength()
	{
		return _strength;
	}
	
	unsigned short int Character::constitution()
	{
		return _constitution;
	}
	
	unsigned short int Character::dexterity()
	{
		return _dexterity;
	}
	
	unsigned short int Character::intelligence()
	{
		return _intelligence;
	}
	
	unsigned short int Character::wisdom()
	{
		return _wisdom;
	}
	
	unsigned short int Character::charisma()
	{
		return _charisma;
	}

	std::string Character::name()
	{
		return _name;
	}
	
	Tile* Character::get_location()
	{
		sql::Connection* conn = _parent->sql()->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `Tiles`.`id` FROM `Characters` JOIN `Tiles` WHERE `Characters`.`tile_id` = `Tiles`.`id` AND `Characters`.`id` = ?");
		prep_stmt->setUInt(1, id());
		sql::ResultSet* res = prep_stmt->executeQuery();
		res->next();
		
		Tile* tile = _parent->get_tile(res->getUInt(1));
		
		delete res;
		delete prep_stmt;
		delete conn;
		
		return tile;
	}
	
	void Character::say(const std::string& msg)
	{
		get_location()->broadcast(name() + " says: \"" + msg + "\"");
	}
	
	void Character::emote(const std::string& msg, bool possessive)
	{
		if (possessive)
		{
			get_location()->broadcast(name() + "'s " + msg);
		}
		else
		{
			get_location()->broadcast(name() + " " + msg);
		}
		
	}
	
	Tile* Character::move(const std::string& direction)
	{		
		Tile* location = get_location();
		
		Tile* destination = location->can_exit(direction);
		if (!destination)
		{
			return 0;
		}
		else
		{
			location->broadcast_except(this, name() + " went " + direction + ".");
			update_location(destination);
			get_location()->broadcast_except(this, name() + " arrived from the " + Tile::opposite_direction(direction) + ".");
		}
		return destination;
	}
	
	std::string Character::look()
	{
		std::stringstream sstream;
		Tile* location = get_location();
		sstream << "You look around and see: " << location->description() << "\n";
		//get who else is here
		std::vector<Character*> occupants = location->get_occupants();
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
			if (n == exits.size()-1)
				sstream << exits[n] << "\n";
			else
				sstream << exits[n] << ", ";
		}
		//Items
		Inventory* tileinv = location->get_inventory();
		if (tileinv)
		{
			sstream << tileinv->describe_contents() << "\n";
		}
		return sstream.str();
	}
	
	std::string Character::pickup(const std::string& target, unsigned int targetn, const std::string& destination)
	{
		Tile* location = get_location();
		Inventory* tileinv = location->get_inventory();
		ItemInstance* targetitem = tileinv->select(target, targetn);
		
		if (targetitem)
		{
			Inventory* myinv = get_inventory(destination);
			if (myinv)
			{
				if (myinv->acquire(targetitem))
				{
					location->broadcast(name() + " picked up a " + target + ".");
					return "You picked up a " + target + " and put it in your " + destination + ".";
				}
				else
				{
					return "You can't put the " + target + " in your " + destination + ".";
				}
			}
			else
			{
				return "You don't have an inventory named '" + destination + "'.";
			}
		}
		return "Unable to find any item named '" + target + "'.";
	}
	
	std::string Character::drop(const std::string& target, unsigned int targetn, const std::string& origin)
	{
		Inventory* inv = get_inventory(origin);
		if (!inv)
		{
			return "You don't have an inventory named '" + origin + "'.";
		}
		
		ItemInstance* targetitem = inv->select(target, targetn);
		
		if (targetitem)
		{
			Tile* location = get_location();
			Inventory* tileinv = location->get_inventory();
			if (tileinv->acquire(targetitem))
			{
				location->broadcast(name() + " dropped a " + target + " on the ground.");
				return "You dropped a " + target + " from your " + origin + ".";
			}
			else
			{
				return "You can't drop the " + target + ".";
			}
		}
		return "Unable to find any item named '" + target + "'.";
	}
	
	std::string Character::examine(const std::string& origin, const std::string& target, unsigned int targetn)
	{
		Inventory* origininv = 0;
		if (origin == "ground")
		{
			origininv = get_location()->get_inventory();
		}
		else
		{
			origininv = get_inventory(origin);
		}
		
		if (!origininv)
		{
			return "Unable to locate that inventory";
		}
		
		ItemInstance* targetitem = origininv->select(target, targetn);
		
		if (!targetitem)
		{
			return "Unable to find any item named '" + target + "'.";
		}
		
		std::stringstream out;
		out << "You examine the " + targetitem->type()->name() << ": " << targetitem->type()->description() << "\n";
		out << "It looks to be about " << targetitem->type()->size() << " cubic meters, and " << targetitem->type()->mass() << "kg";
		return out.str();
	}
	
	Inventory* Character::get_inventory(const std::string& name)
	{
		sql::Connection* conn = _parent->sql()->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `id` FROM `Inventories` WHERE `char_id` = ? AND `name` = ?");
		prep_stmt->setUInt(1, id());
		prep_stmt->setString(2, name);
		sql::ResultSet* res = prep_stmt->executeQuery();
		if (res->rowsCount() == 0)
		{
			delete res;
			delete prep_stmt;
			delete conn;
			return 0;
		}
		
		res->next();
		Inventory* inventory = _parent->get_inventory(res->getUInt(1));
		
		delete res;
		delete prep_stmt;
		delete conn;
		
		return inventory;
	}
	
	Inventory* Character::add_inventory(const std::string& name, unsigned short int capacity)
	{
		if (get_inventory(name))
			return 0; //Inventory already exists, error
		
		Inventory* inventory = Inventory::build(_parent, this, name, capacity);
		return inventory;
	}
	
	std::vector<Inventory*> Character::all_inventories()
	{
		std::vector<Inventory*> out;
		
		sql::Connection* conn = _parent->sql()->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `id` FROM `Inventories` WHERE `char_id` = ?");
		prep_stmt->setUInt(1, id());
		sql::ResultSet* res = prep_stmt->executeQuery();
		
		while (res->next())
		{
			out.push_back(_parent->get_inventory(res->getUInt(1)));
		}
		
		delete res;
		delete prep_stmt;
		delete conn;
		
		return out;
	}
	
	int Character::carrying_mass()
	{
		int mass = 0;
		
		sql::Connection* conn = _parent->sql()->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT SUM(`ItemTypes`.`mass`) FROM `Inventories` JOIN `ItemInstances` JOIN `ItemTypes` WHERE `Inventories`.`id` = `ItemInstances`.`inv_id` AND `ItemInstances`.`type_id` = `ItemTypes`.`id` AND `Inventories`.`char_id` = ?");
		prep_stmt->setUInt(1, id());
		sql::ResultSet* res = prep_stmt->executeQuery();
		
		res->next();
		
		mass = res->getInt(1);
		
		delete res;
		delete prep_stmt;
		delete conn;
		
		return mass;
	}
	
	int Character::max_carrying_mass()
	{
		return 100;
	}
	
	unsigned int Character::id()
	{
		return _id;
	}
	
	void Character::update_location(Tile* destination)
	{
		sql::Connection* conn = _parent->sql()->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("UPDATE `Characters` SET `tile_id` = ? WHERE `id` = ?");
		prep_stmt->setUInt(1, destination->id());
		prep_stmt->setUInt(2, id());
		prep_stmt->execute();
		
		delete prep_stmt;
		delete conn;
	}
}
}
