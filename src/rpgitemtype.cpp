#include "rpgitemtype.h"
#include "rpggame.h"
#include "mysql.h"

namespace teh
{
	RPGItemType* RPGItemType::build(RPGGame* parent, const std::string& name, const std::string& summary, const std::string& description, unsigned short int size, unsigned short int mass)
	{
		sql::Connection* conn = parent->sql()->connect();
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("INSERT INTO `ItemTypes` VALUES (NULL, ?, ?, ?, ?, ?)");
		prep_stmt->setString(1, name);
		prep_stmt->setString(2, summary);
		prep_stmt->setString(3, description);
		prep_stmt->setUInt(4, size);
		prep_stmt->setUInt(5, mass);
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
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `name`, `summary`, `description`, `size`, `mass` FROM `ItemTypes` WHERE `id` = ?");
		prep_stmt->setUInt(1, _id);
		sql::ResultSet* res = prep_stmt->executeQuery();
		res->next();
		_name = res->getString("name");
		_summary = res->getString("summary");
		_description = res->getString("description");
		_size = res->getUInt("size");
		_mass = res->getUInt("mass");
		
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
	
	std::string RPGItemType::summary()
	{
		return _summary;
	}
	
	std::string RPGItemType::description()
	{
		return _description;
	}
	
	unsigned short int RPGItemType::size()
	{
		return _size;
	}
	
	unsigned short int RPGItemType::mass()
	{
		return _mass;
	}
}