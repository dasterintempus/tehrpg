#pragma once

#include <SFML/System.hpp>

#include <sstream>

#include "gameserver_interface.h"

namespace teh
{
	class ConsoleConnection
		: public GameConnectionInterface
	{
		public:
			ConsoleConnection();
		
			void start();
			void finish();
		
			void add_in_line(const std::string& line);
		
			bool has_line();
			std::string read_line();
			
			void write_line(const std::string& line);
		
			void close();
		private:
			bool _done;
			sf::Mutex _donemutex;
			std::stringstream _inbuffer;
			sf::Mutex _inbuffermutex;
	};
}