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
		
		class InvalidSystem
			: public std::exception
		{
			public:
				InvalidSystem(const std::string& name);
				virtual const char* what() const throw();
				std::string name;
		};
	}
}