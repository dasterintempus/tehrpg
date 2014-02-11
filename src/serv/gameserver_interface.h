#pragma once

#include <string>

#include "typedefs.h"

namespace teh
{
	class GameConnectionInterface
	{
		public:
			virtual bool has_line() = 0;
			virtual std::string read_line() = 0;
			
			virtual void write_line(const std::string& line) = 0;
		
			//called by GameServerInterface to start shutdown
			virtual void close() = 0;
	};
	
	class GameServerInterface
	{
		public:
			virtual clientid add_connection(GameConnectionInterface* conn) = 0;
		
			//virtual void remove_client(GameClientInterface* client) = 0;
			virtual void remove_client(const clientid& id) = 0;
			
			//virtual void close_client(GameClientInterface* client) = 0;
			virtual void close_client(const clientid& id) = 0;
	};
}