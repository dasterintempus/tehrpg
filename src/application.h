#pragma once

#include <SFML/System.hpp>
#include "typedefs.h"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace teh
{	
	class NetServer;
	class GameServer;
	class MySQL;
	namespace RPG
	{
		class Game;
	}
	class CommandParser;
	class ConsoleConnection;
	
	class Application
	{
		public:
			Application();
			~Application();
		
			int init(int argc, char** argv);
			int start(int argc, char** argv);
			void finish();

			MySQL* sql();
			RPG::Game* rpg();
			CommandParser* parser();
		private:
			void start_gameserver();
			void start_netserver();
		
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
			RPG::Game* _rpggame;
		
			CommandParser* _commandparser;
		
			po::variables_map _vm;
	};
}
