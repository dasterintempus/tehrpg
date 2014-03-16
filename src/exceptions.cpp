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
			msg << "Entity of id# " << id << " was allocated but not found in database.";
			return msg.str().c_str();
		}
		
		ComponentNotFound::ComponentNotFound(const std::string& componenttype, unsigned int entityid)
			: componenttype(componenttype), entityid(entityid)
		{
			
		}
		
		const char* ComponentNotFound::what() const throw()
		{
			std::stringstream msg;
			msg << "Component of type " << componenttype << " on entity id# " << entityid << " was allocated but not found in database.";
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
		
		InvalidComponentDefinition::InvalidComponentDefinition(const std::string& component, const std::string& fieldname, const std::string& fieldtype)
			: component(component), fieldname(fieldname), fieldtype(fieldtype)
		{
			
		}
		
		const char* InvalidComponentDefinition::what() const throw()
		{
			std::stringstream msg;
			msg << "Component definition named '" << component << "' had field of name '" << fieldname << "' with invalid type '" << fieldtype << "'.";
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
		
		SystemLuaError::SystemLuaError(const std::string& msg)
			: msg(msg)
		{
			
		}
		
		const char* SystemLuaError::what() const throw()
		{
			return msg.c_str();
		}
	}
}