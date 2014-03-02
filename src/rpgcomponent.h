#pragma once

#include <vector>
#include <string>
#include <map>
#include <utility>

namespace teh
{
	namespace RPG
	{
		class Entity;
		class Engine;
		
		class Component
		{
			public:
				enum ValueType
				{
					TypeInt,
					TypeUInt,
					TypeString,
					TypeBool,
					TypeInvalid,
				};
				
				typedef std::vector<std::pair<std::string, ValueType> > Schema;
				
				static Schema getSchema(const std::string& componenttype, Engine* engine);
				static ValueType getSchemaKeyType(const std::string& componenttype, Engine* engine, const std::string& key);
			
				Component(const std::string& componenttype, Entity* parent, Engine* engine);
				virtual ~Component();
			
				std::string componenttype() const;
				unsigned int id() const;
			
				std::string getString(const std::string& key) const;
				bool hasString(const std::string& key) const;
				void setString(const std::string& key, const std::string& value);
			
				long int getInt(const std::string& key) const;
				bool hasInt(const std::string& key) const;
				void setInt(const std::string& key, const long int& value);
			
				unsigned int getUInt(const std::string& key) const;
				bool hasUInt(const std::string& key) const;
				void setUInt(const std::string& key, const unsigned int& value);
			
				bool getBool(const std::string& key) const;
				bool hasBool(const std::string& key) const;
				void setBool(const std::string& key, const bool& value);
				
			private:
				unsigned int _id;	
				std::string _componenttype;				
				Entity* _parent;
				Engine* _engine;
				std::map<std::string, std::string> _strings;
				std::map<std::string, unsigned int> _uints;
				std::map<std::string, long int> _ints;
				std::map<std::string, bool> _bools;
		};
	}
}