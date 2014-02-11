#pragma once

#include <boost/program_options.hpp>
#include <SFML/System.hpp>
#include "typedefs.h"
#include "mysql.h"

namespace teh
{
	class NetServer;
	class GameServer;

	class ConsoleConnection;
	
	class Application
	{
		public:
			Application(int argc, char** argv);
			~Application();
		
			void start();
			void finish();

			MySQL* sql();
		private:
			void start_gameserver();
			void start_netserver();
		
			boost::program_options::variables_map _cliargs;
		
			sf::Thread* _gameserverthread;
			sf::Thread* _netserverthread;
			sf::Thread* _consolethread;
		
			GameServer* _gameserver;
			NetServer* _netserver;
		
			ConsoleConnection* _consoleconnection;
		
			bool _done;
			sf::Mutex _donemutex;

			MySQL* _mysql;
	};
}
