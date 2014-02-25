#include "exceptions.h"
#include <sstream>

namespace teh
{
	namespace Exceptions
	{
		InvalidObject::InvalidObject(unsigned int id)
			: _id(id)
		{
			
		}
		
		const char* InvalidObject::what() const throw()
		{
			std::stringstream msg;
			msg << "Invalid object of id#" << _id << " was allocated.";
			return msg.str().c_str();
		}
	}
}