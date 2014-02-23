#include "rpgitemtype.h"
#include "rpggame.h"
#include "mysql.h"

namespace teh
{
	RPGItemType* RPGItemType::build(RPGGame* parent, const std::string& name, const std::string& description, unsigned short int size, bool stackable)
	{
		sql::Connection* conn = parent->sql()->connect();
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("INSERT INTO `ItemTypes` VALUES (NULL, ?, ?, ?, ?)");
		prep_stmt->setString(1, name);
		prep_stmt->setString(2, description);
		prep_stmt->setUInt(3, size);
		prep_stmt->setBoolean(4, stackable);
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
		
		return parent->get_itemtype(id);
	}
	
	RPGItemType::RPGItemType(unsigned int id, RPGGame* parent)
		: _id(id), _parent(parent)
	{
		sql::Connection* conn = _parent->sql()->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `name`, `description`, `size` FROM `ItemTypes` WHERE `id` = ?");
		prep_stmt->setUInt(1, _id);
		sql::ResultSet* res = prep_stmt->executeQuery();
		res->next();
		_name = res->getString("name");
		_description = res->getString("description");
		_size = res->getUInt("size");
		
		delete res;
		delete prep_stmt;
		delete conn;
	}
	
	unsigned int RPGItemType::id()
	{
		return _id;
	}
	
	std::string RPGItemType::name()
	{
		return _name;
	}
	
	std::string RPGItemType::description()
	{
		return _description;
	}
	
	unsigned short int RPGItemType::size()
	{
		return _size;
	}
}