#include "rpgentity.h"
#include "rpgcomponent.h"
#include "rpgengine.h"
#include "mysql.h"
#include "exceptions.h"

namespace teh
{
	namespace RPG
	{
		Entity::Entity(const std::string& name, const std::string& type, Engine* engine)
			: _name(name), _type(type), _engine(engine), _id(0)
		{
			build();
		}
		
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
				try
				{
					_components[componenttype] = new Component(componenttype, this, _engine);
				}
				catch (teh::Exceptions::ComponentNotFound& e)
				{
					return 0;
				}
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
		
		void Entity::destroy()
		{
			if (_id == 0)
				return;
			
			sql::Connection* conn = _engine->sql()->connect();
			
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("DELETE FROM `Entities` WHERE id = ?");
			prep_stmt->setUInt(1, _id);
			
			prep_stmt->execute();
			
			_engine->delete_entity(this);
		}
		
		void Entity::takeownership(Component* component)
		{
			if (_components.count(component->componenttype()) == 0)
			{
				_components[component->componenttype()] = component;
			}
		}
		
		void Entity::detach(const std::string& componenttype)
		{
			Component* c = component(componenttype);
			if (!c)
				return;
			c->destroy();
			delete c;
			_components.erase(componenttype);
		}
		
		unsigned int Entity::userid()
		{
			return _userid;
		}
		
		void Entity::userid(unsigned int id)
		{
			_userid = id;
			sql::Connection* conn = _engine->sql()->connect();
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("UPDATE `Entities` SET `user_id` = ? WHERE `id` = ?");
			prep_stmt->setUInt(1, _userid);
			prep_stmt->setUInt(2, _id);
			
			prep_stmt->execute();
			delete prep_stmt;
			delete conn;
		}
		
		void Entity::build()
		{
			sql::Connection* conn = _engine->sql()->connect();
			
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("INSERT INTO `Entities` VALUES (NULL, ?, ?, NULL)");
			prep_stmt->setString(1, _name);
			prep_stmt->setString(2, _type);
			
			prep_stmt->execute();
			
			delete prep_stmt;
			
			//get the entity id
			prep_stmt = conn->prepareStatement("SELECT LAST_INSERT_ID()");
			sql::ResultSet* res = prep_stmt->executeQuery();
			res->next();
			
			_id = res->getUInt(1);
			
			delete res;
			delete prep_stmt;
			
			delete conn;
			
			_engine->activate_entity(this);
		}
	}	
}
