#include "rpginventory.h"
#include "mysql.h"
#include "rpggame.h"
#include "rpgtile.h"
#include "rpgcharacter.h"
#include "rpgiteminstance.h"

namespace teh
{
	RPGInventory* RPGInventory::build(RPGGame* parent, RPGTile* tile)
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
	
	RPGInventory* RPGInventory::build(RPGGame* parent, RPGCharacter* character, std::string name, unsigned short int capacity)
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

	RPGInventory::RPGInventory(unsigned int id, RPGGame* parent)
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
	
	unsigned int RPGInventory::id()
	{
		return _id;
	}
	
	std::string RPGInventory::name()
	{
		return _name;
	}
	
	unsigned short int RPGInventory::capacity()
	{
		return _capacity;
	}
	
	RPGCharacter* RPGInventory::character()
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
	
	RPGTile* RPGInventory::tile()
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
	
	std::vector<RPGItemInstance*> RPGInventory::contents()
	{
		std::vector<RPGItemInstance*> out;
		
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
	
	unsigned short int RPGInventory::space_used()
	{
		sql::Connection* conn = _parent->sql()->connect();
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT SUM(`ItemTypes`.`size`) FROM `ItemInstances` JOIN `ItemTypes` WHERE `ItemTypes`.`id` = `ItemInstances`.`type_id` AND `ItemInstances`.`inv_id` = ?");
		prep_stmt->setUInt(1, id());
		sql::ResultSet* res = prep_stmt->executeQuery();
		res->next();
		return res->getUInt(1);
	}
	
	unsigned short int RPGInventory::space_remaining()
	{
		if (space_used() > capacity())
			return 0;
		return capacity() - space_used();
	}
}