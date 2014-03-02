#include "exceptions.h"
#include <sstream>

namespace teh
{
	namespace Exceptions
	{
		EntityNotFound::EntityNotFound(unsigned int id)
			: id(id)
		{
			
		}
		
		const char* EntityNotFound::what() const throw()
		{
			std::stringstream msg;
			msg << "Entity of id#" << id << " was allocated but not found in database.";
			return msg.str().c_str();
		}
		
		ComponentSchemaViolation::ComponentSchemaViolation(unsigned int id, const std::string& key, const std::string& type)
			: id(id), key(key), type(type)
		{
			
		}
		
		const char* ComponentSchemaViolation::what() const throw()
		{
			std::stringstream msg;
			msg << "Component of id#" << id << " attempted to get/set non-existent key '" << key << "' of type '" << type << "'.";
			return msg.str().c_str();
		}
		
		InvalidSystem::InvalidSystem(const std::string& name)
			: name(name)
		{
			
		}
		
		const char* InvalidSystem::what() const throw()
		{
			std::stringstream msg;
			msg << "System with name " << name << " was allocated but not found on filesystem.";
			return msg.str().c_str();
		}
	}
}