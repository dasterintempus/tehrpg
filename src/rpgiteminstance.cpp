#include "rpgiteminstance.h"
#include "mysql.h"
#include "rpginventory.h"
#include "rpgitemtype.h"
#include "rpggame.h"

namespace teh
{
	RPGItemInstance* RPGItemInstance::build(RPGGame* parent, RPGInventory* inv, RPGItemType* type)
	{
		if (inv->capacity() != 0 && inv->space_remaining() < type->size())
			return 0;
		
		sql::Connection* conn = parent->sql()->connect();
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("INSERT INTO `ItemInstances` VALUES (NULL, ?, ?)");
		prep_stmt->setUInt(1, inv->id());
		prep_stmt->setUInt(2, type->id());
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
		
		return parent->get_iteminstance(id);
	}
	
	RPGItemInstance::RPGItemInstance(unsigned int id, RPGGame* parent)
		: _id(id), _parent(parent)
	{
		//No data on this object... yet
	}
	
	unsigned int RPGItemInstance::id()
	{
		return _id;
	}
	
	RPGInventory* RPGItemInstance::container()
	{
		sql::Connection* conn = _parent->sql()->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `inv_id` FROM `ItemInstances` WHERE `id` = ?");
		prep_stmt->setUInt(1, id());
		sql::ResultSet* res = prep_stmt->executeQuery();
		res->next();
		RPGInventory* inv = _parent->get_inventory(res->getUInt(1));
		
		delete res;
		delete prep_stmt;
		delete conn;
		
		return inv;
	}
	
	RPGItemType* RPGItemInstance::type()
	{
		sql::Connection* conn = _parent->sql()->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `type_id` FROM `ItemInstances` WHERE `id` = ?");
		prep_stmt->setUInt(1, id());
		sql::ResultSet* res = prep_stmt->executeQuery();
		res->next();
		
		RPGItemType* type = _parent->get_itemtype(res->getUInt(1));
		
		delete res;
		delete prep_stmt;
		delete conn;
		
		return type;
	}
	
}