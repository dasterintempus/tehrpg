#pragma once
#include <exception>
#include <string>

namespace teh
{
	namespace Exceptions
	{
		class EntityNotFound
			: public std::exception
		{
			public:
				EntityNotFound(unsigned int id);
				virtual const char* what() const throw();
				unsigned int id;
		};
		
		class ComponentNotFound
			: public std::exception
		{
			public:
				ComponentNotFound(const std::string& componenttype, unsigned int entityid);
				virtual const char* what() const throw();
				std::string componenttype;
				unsigned int entityid;
		};
		
		class ComponentSchemaViolation
			: public std::exception
		{
			public:
				ComponentSchemaViolation(unsigned int id, const std::string& key, const std::string& type);
				virtual const char* what() const throw();
				unsigned int id;
				std::string key;
				std::string type;
		};
		
		class InvalidComponentDefinition
			: public std::exception
		{
			public:
				InvalidComponentDefinition(const std::string& component, const std::string& fieldname, const std::string& fieldtype);
				virtual const char* what() const throw();
				std::string component;
				std::string fieldname;
				std::string fieldtype;
		};
		
		class InvalidSystem
			: public std::exception
		{
			public:
				InvalidSystem(const std::string& name);
				virtual const char* what() const throw();
				std::string name;
		};
		
		class SystemLuaError
			: public std::exception
		{
			public:
				SystemLuaError(const std::string& msg);
				virtual const char* what() const throw();
				std::string msg;
		};
	}
}