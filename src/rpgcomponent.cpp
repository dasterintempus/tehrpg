#include "rpgcomponent.h"
#include "rpgengine.h"
#include "rpgentity.h"
#include "mysql.h"
#include "exceptions.h"

namespace teh
{
	namespace RPG
	{
		Component::Schema Component::getSchema(const std::string& componenttype, Engine* engine)
		{
			Schema out;
			std::string tablename = componenttype + "Components";
			
			sql::Connection* conn = engine->sql()->connect();
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT * FROM `" + tablename + "` LIMIT 1");
			sql::ResultSet* res = prep_stmt->executeQuery();
			sql::ResultSetMetaData* meta = res->getMetaData();
			
			for (unsigned int column = 1; column <= meta->getColumnCount(); column++)
			{
				std::string colname = meta->getColumnName(column);
				if (colname == "id" || colname == "entity_id")
				{
					continue;
				}
				std::string coltypename = meta->getColumnTypeName(column);
				if (coltypename == "VARCHAR")
				{
					out.push_back(std::make_pair(colname, TypeString));
				}
				else if (coltypename == "INT")
				{
					out.push_back(std::make_pair(colname, TypeInt));
				}
				else if (coltypename == "INT UNSIGNED")
				{
					out.push_back(std::make_pair(colname, TypeUInt));
				}
				else if (coltypename == "BOOLEAN")
				{
					out.push_back(std::make_pair(colname, TypeBool));
				}
			}
			
			delete res;
			delete prep_stmt;
			delete conn;
			
			return out;
		}
		
		Component::ValueType Component::getSchemaKeyType(const std::string& componenttype, Engine* engine, const std::string& key)
		{
			Schema schema = getSchema(componenttype, engine);
			
			for (unsigned int n = 0;n < schema.size(); n++)
			{
				std::string schemakey = schema[n].first;
				ValueType vt = schema[n].second;
				
				if (schemakey == key)
				{
					return vt;
				}
			}
			return TypeInvalid;
		}
		
		Component::Component(const std::string& componenttype, Entity* parent, Engine* engine)
			: _id(0), _componenttype(componenttype), _parent(parent), _engine(engine)
		{
			std::string tablename = _componenttype + "Components";
			
			sql::Connection* conn = _engine->sql()->connect();
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT * FROM `" + tablename + "` WHERE `entity_id` = ?");
			prep_stmt->setInt(1, _parent->id());
			sql::ResultSet* res = prep_stmt->executeQuery();
			sql::ResultSetMetaData* meta = res->getMetaData();
			res->next();
			
			_id = res->getUInt("id");
			
			for (unsigned int column = 1; column <= meta->getColumnCount(); column++)
			{
				std::string colname = meta->getColumnName(column);
				if (colname == "id" || colname == "entity_id")
				{
					continue;
				}
				std::string coltypename = meta->getColumnTypeName(column);
				if (coltypename == "VARCHAR")
				{
					std::string value = res->getString(colname.c_str());
					_strings[colname] = value;
				}
				else if (coltypename == "INT")
				{
					long int value = res->getInt(colname.c_str());
					_ints[colname] = value;
				}
				else if (coltypename == "INT UNSIGNED")
				{
					unsigned int value = res->getUInt(colname.c_str());
					_uints[colname] = value;
				}
				else if (coltypename == "BOOLEAN")
				{
					bool value = res->getBoolean(colname.c_str());
					_bools[colname] = value;
				}
			}
			
			delete res;
			delete prep_stmt;
			delete conn;
		}
		
		Component::~Component()
		{
			
		}
		
		std::string Component::componenttype() const
		{
			return _componenttype;
		}
		
		unsigned int Component::id() const
		{
			return _id;
		}
		
		std::string Component::getString(const std::string& key) const
		{
			if (hasString(key))
				return _strings.at(key);
			throw teh::Exceptions::ComponentSchemaViolation(_id, key, "string");
			return std::string();
		}
		
		bool Component::hasString(const std::string& key) const
		{
			return _strings.count(key) != 0;
		}
		
		void Component::setString(const std::string& key, const std::string& value)
		{
			if (!hasString(key))
				throw teh::Exceptions::ComponentSchemaViolation(_id, key, "string");
			_strings[key] = value;
			
			std::string tablename = componenttype() + "Components";
			
			sql::Connection* conn = _engine->sql()->connect();
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("UPDATE `" + tablename + "` SET `" + key + "` = ? WHERE `id` = ?");
			prep_stmt->setString(1, value);
			prep_stmt->setUInt(2, id());
			prep_stmt->execute();
		}
	
		long int Component::getInt(const std::string& key) const
		{
			if (hasInt(key))
				return _ints.at(key);
			throw teh::Exceptions::ComponentSchemaViolation(_id, key, "int");
			return 0;
		}
		
		bool Component::hasInt(const std::string& key) const
		{
			return _ints.count(key) != 0;
		}
		
		void Component::setInt(const std::string& key, const long int& value)
		{
			if (!hasInt(key))
				throw teh::Exceptions::ComponentSchemaViolation(_id, key, "int");
			_ints[key] = value;
			
			std::string tablename = componenttype() + "Components";
			
			sql::Connection* conn = _engine->sql()->connect();
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("UPDATE `" + tablename + "` SET `" + key + "` = ? WHERE `id` = ?");
			prep_stmt->setInt(1, value);
			prep_stmt->setUInt(2, id());
			prep_stmt->execute();
		}
	
		unsigned int Component::getUInt(const std::string& key) const
		{
			if (hasUInt(key))
				return _uints.at(key);
			throw teh::Exceptions::ComponentSchemaViolation(_id, key, "uint");
			return 0;
		}
		
		bool Component::hasUInt(const std::string& key) const
		{
			return _uints.count(key) != 0;
		}
		
		void Component::setUInt(const std::string& key, const unsigned int& value)
		{
			if (!hasUInt(key))
				throw teh::Exceptions::ComponentSchemaViolation(_id, key, "uint");
			_uints[key] = value;
			
			std::string tablename = componenttype() + "Components";
			
			sql::Connection* conn = _engine->sql()->connect();
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("UPDATE `" + tablename + "` SET `" + key + "` = ? WHERE `id` = ?");
			prep_stmt->setUInt(1, value);
			prep_stmt->setUInt(2, id());
			prep_stmt->execute();
		}
	
		bool Component::getBool(const std::string& key) const
		{
			if (hasBool(key))
				return _bools.at(key);
			throw teh::Exceptions::ComponentSchemaViolation(_id, key, "bool");
			return false;
		}
		
		bool Component::hasBool(const std::string& key) const
		{
			return _bools.count(key) != 0;
		}
		
		void Component::setBool(const std::string& key, const bool& value)
		{
			if (!hasBool(key))
				throw teh::Exceptions::ComponentSchemaViolation(_id, key, "bool");
			_bools[key] = value;
			
			std::string tablename = componenttype() + "Components";
			
			sql::Connection* conn = _engine->sql()->connect();
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("UPDATE `" + tablename + "` SET `" + key + "` = ? WHERE `id` = ?");
			prep_stmt->setBoolean(1, value);
			prep_stmt->setUInt(2, id());
			prep_stmt->execute();
		}
	}
}