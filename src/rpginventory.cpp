#include "rpginventory.h"
#include "mysql.h"
#include "rpggame.h"
#include "rpgtile.h"
#include "rpgcharacter.h"
#include "rpgiteminstance.h"
#include "rpgitemtype.h"

#include <sstream>

namespace teh
{
namespace RPG
{
	Inventory* Inventory::build(Game* parent, Tile* tile)
	{
		sql::Connection* conn = parent->sql()->connect();
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("INSERT INTO `Inventories` VALUES (NULL, NULL, DEFAULT, NULL, ?)");
		prep_stmt->setUInt(1, tile->id());
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
		
		return parent->get_inventory(id);
	}
	
	Inventory* Inventory::build(Game* parent, Character* character, std::string name, unsigned short int capacity)
	{
		if (name == "" || capacity == 0)
		{
			return 0;
		}
		
		sql::Connection* conn = parent->sql()->connect();
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("INSERT INTO `Inventories` VALUES (NULL, ?, ?, ?, NULL)");
		prep_stmt->setString(1, name);
		prep_stmt->setUInt(2, capacity);
		prep_stmt->setUInt(3, character->id());
		
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
		
		return parent->get_inventory(id);
	}

	Inventory::Inventory(unsigned int id, Game* parent)
		: _id(id), _parent(parent)
	{
		sql::Connection* conn = _parent->sql()->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `name`, `capacity` FROM `Inventories` WHERE `id` = ?");
		prep_stmt->setUInt(1, _id);
		sql::ResultSet* res = prep_stmt->executeQuery();
		res->next();
		_name = res->getString("name");
		_capacity = res->getUInt("capacity");
		
		delete res;
		delete prep_stmt;
		delete conn;
	}
	
	unsigned int Inventory::id()
	{
		return _id;
	}
	
	std::string Inventory::name()
	{
		return _name;
	}
	
	unsigned short int Inventory::capacity()
	{
		return _capacity;
	}
	
	Character* Inventory::character()
	{
		sql::Connection* conn = _parent->sql()->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `char_id` FROM `Inventories` WHERE `id` = ? AND `char_id` != NULL");
		prep_stmt->setUInt(1, id());
		sql::ResultSet* res = prep_stmt->executeQuery();
		if (res->rowsCount() == 0)
			return 0;
		
		res->next();
		
		unsigned int charid = res->getUInt(1);
		
		delete res;
		delete prep_stmt;
		delete conn;
		
		return _parent->get_character(charid);
	}
	
	Tile* Inventory::tile()
	{
		sql::Connection* conn = _parent->sql()->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `tile_id` FROM `Inventories` WHERE `id` = ? AND `tile_id` != NULL");
		prep_stmt->setUInt(1, id());
		sql::ResultSet* res = prep_stmt->executeQuery();
		if (res->rowsCount() == 0)
			return 0;
		
		res->next();
		
		unsigned int tileid = res->getUInt(1);
		
		delete res;
		delete prep_stmt;
		delete conn;
		
		return _parent->get_tile(tileid);
	}
	
	std::vector<ItemInstance*> Inventory::contents()
	{
		std::vector<ItemInstance*> out;
		
		sql::Connection* conn = _parent->sql()->connect();
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `id` FROM `ItemInstances` WHERE `inv_id` = ? ORDER BY `id`");
		prep_stmt->setUInt(1, id());
		sql::ResultSet* res = prep_stmt->executeQuery();
		while (res->next())
		{
			out.push_back(_parent->get_iteminstance(res->getUInt(1)));
		}
		
		return out;
	}
	
	unsigned short int Inventory::space_used()
	{
		sql::Connection* conn = _parent->sql()->connect();
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT SUM(`ItemTypes`.`size`) FROM `ItemInstances` JOIN `ItemTypes` WHERE `ItemTypes`.`id` = `ItemInstances`.`type_id` AND `ItemInstances`.`inv_id` = ?");
		prep_stmt->setUInt(1, id());
		sql::ResultSet* res = prep_stmt->executeQuery();
		res->next();
		return res->getUInt(1);
	}
	
	unsigned short int Inventory::space_remaining()
	{
		if (space_used() > capacity())
			return 0;
		return capacity() - space_used();
	}
	
	bool Inventory::acquire(ItemInstance* item)
	{
		if (capacity() != 0)
		{
			if (space_remaining() < item->type()->size())
				return false;
		}
		
		Character* c = character();
		if (c)
		{
			if (c->carrying_mass() + item->type()->mass() > c->max_carrying_mass())
				return false;
		}
		
		sql::Connection* conn = _parent->sql()->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("UPDATE `ItemInstances` SET `inv_id` = ? WHERE `id` = ?");
		prep_stmt->setUInt(1, id());
		prep_stmt->setUInt(2, item->id());
		
		try
		{
			prep_stmt->execute();
		}
		catch (sql::SQLException& e)
		{
			delete prep_stmt;
			delete conn;
			return false;
		}
		
		delete prep_stmt;
		delete conn;
		return true;
	}
	
	ItemInstance* Inventory::select(const std::string& target, unsigned int targetn)
	{
		ItemInstance* selected = 0;
		std::map<ItemType*, unsigned int> typecounters;
		std::vector<ItemInstance*> c = contents();
		bool multiple = false;
		for (unsigned int n = 0; n < c.size(); n++)
		{
			ItemInstance* item = c[n];
			ItemType* type = item->type();
			
			if (typecounters.count(type)==0)
				typecounters[type] = 0;
			
			typecounters[type]++;

			if (target == type->name())
			{
				if (typecounters[type] > 1)
					multiple = true;
				if (targetn > 0)
				{
					if (targetn == typecounters[type])
					{
						selected = item;
					}
				}
				else
				{
					selected = item;
				}
			}
		}
		return selected;
	}
	
	std::string Inventory::describe_contents()
	{
		std::stringstream sstream;
		std::map<ItemType*, unsigned int> typecounters;
		std::vector<ItemInstance*> c = contents();
		if (c.size() > 0)
		{
			sstream << "There are the following items: ";
		}
		else
		{
			sstream << "There are no items.";
		}
		for (unsigned int n = 0; n < c.size(); n++)
		{
			ItemInstance* item = c[n];
			ItemType* type = item->type();
			
			if (typecounters.count(type)==0)
				typecounters[type] = 0;
			
			typecounters[type]++;
	
			if (n == c.size() - 1)
				sstream << type->summary() << " (" << typecounters[type] << ").";
			else
				sstream << type->summary() << " (" << typecounters[type] << "), ";
		}
		return sstream.str();
	}
}
}