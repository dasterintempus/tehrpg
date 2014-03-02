#include "rpgentity.h"
#include "rpgcomponent.h"
#include "rpgengine.h"
#include "mysql.h"
#include "exceptions.h"

namespace teh
{
	namespace RPG
	{
		Entity::Entity(unsigned int id, Engine* engine)
			: _id(id), _engine(engine)
		{
			sql::Connection* conn = _engine->sql()->connect();
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `name`, `type` FROM `Entities` WHERE `id` = ?");
			prep_stmt->setInt(1, _id);
			sql::ResultSet* res = prep_stmt->executeQuery();
			if (res->rowsCount() != 1)
			{
				throw teh::Exceptions::EntityNotFound(_id);
			}
			res->next();

			_name = res->getString(1);
			_type = res->getString(2);
			
			delete res;
			delete prep_stmt;
			delete conn;
		}
		
		Entity::~Entity()
		{
			for (auto i = _components.begin(); i != _components.end(); i++)
			{
				delete (*i).second;
			}
		}
		
		Component* Entity::component(const std::string& componenttype)
		{
			if (_components.count(componenttype) == 0)
			{
				_components[componenttype] = new Component(componenttype, this, _engine);
			}
			return _components[componenttype];
		}
		
		unsigned int Entity::id()
		{
			return _id;
		}
		
		std::string Entity::name()
		{
			return _name;
		}
		
		std::string Entity::type()
		{
			return _type;
		}
	}	
}
