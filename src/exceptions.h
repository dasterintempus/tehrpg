#pragma once
#include <exception>

namespace teh
{
	namespace Exceptions
	{
		class InvalidObject
			: public std::exception
		{
			public:
				InvalidObject(unsigned int id);
				virtual const char* what() const throw();
			private:
				unsigned int _id;
		};
	}
}