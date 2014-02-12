#pragma once

#include <boost/program_options.hpp>
#include <SFML/System.hpp>
#include "typedefs.h"

namespace teh
{
	class NetServer;
	class GameServer;
	class MySQL;
	class RPGGame;
	class CommandParser;
	class ConsoleConnection;
	
	class Application
	{
		public:
			Application(int argc, char** argv);
			~Application();
		
			void start();
			void finish();

			MySQL* sql();
			RPGGame* rpg();
			CommandParser* parser();
		private:
			void start_gameserver();
			void start_netserver();
		
			boost::program_options::variables_map _cliargs;
		
			sf::Thread* _gameserverthread;
			sf::Thread* _netserverthread;
			sf::Thread* _rpggamethread;	
			sf::Thread* _consolethread;			
		
			GameServer* _gameserver;
			NetServer* _netserver;
		
			ConsoleConnection* _consoleconnection;
		
			bool _done;
			sf::Mutex _donemutex;

			MySQL* _mysql;
			RPGGame* _rpggame;
		
			CommandParser* _commandparser;
	};
}
