#include "application.h"
#include "netserver.h"
#include "gameserver.h"
#include "consoleconnection.h"
#include "mysql.h"
#include "rpggame.h"
#include "commandparser.h"

#include <iostream>

namespace teh
{
	Application::Application()
		: _gameserver(0), _netserver(0), _rpggame(0),
		_gameserverthread(0),
		_netserverthread(0),
		_rpggamethread(0),
		_consolethread(0),
		_done(false),
		_consoleconnection(0),
		_mysql(0)
	{
		
	}
	
	Application::~Application()
	{
		if (_netserver)
			delete _netserver;
		if (_netserverthread)
			delete _netserverthread;
		if (_gameserver)
			delete _gameserver;
		if (_gameserverthread)
			delete _gameserverthread;
		if (_consoleconnection)
			delete _consoleconnection;
		if (_consolethread)
			delete _consolethread;
		if (_mysql)
			delete _mysql;
		if (_rpggame)
			delete _rpggame;
		if (_rpggamethread)
			delete _rpggamethread;
	}
	
	int Application::init(int argc, char** argv)
	{
		srand(time(0));

		short int port = 3137;
		if (argc >= 2)
		{
			std::string portstr = argv[1];
			if (is_numeric<short int>(portstr))
			{
				port = to_numeric<short int>(portstr);
			}
			else
			{
				std::cerr << "Invalid port specified. (not a number?)" << std::endl;
			return 2;
			}
		}
		
		_gameserver = new GameServer(this);
		_gameserverthread = new sf::Thread(&Application::start_gameserver, this);
		_netserver = new NetServer(port, _gameserver);
		_netserverthread = new sf::Thread(&Application::start_netserver, this);
	
		_consoleconnection = new ConsoleConnection();
		_consolethread = new sf::Thread(&ConsoleConnection::start, _consoleconnection);

		_mysql = new MySQL("localhost", 3306, "tehrpg", "tur7tle", "tehrpg");
	
		_rpggame = new RPGGame(this, _gameserver);
		_rpggamethread = new sf::Thread(&RPGGame::start, _rpggame);
	
		_commandparser = new CommandParser();

		return 0;
	}
  
	int Application::start(int argc, char** argv)
	{
		int result = init(argc, argv);
		if (result != 0)
		{
			return result;
		}

		_gameserverthread->launch();
		_gameserver->add_connection(_consoleconnection);
		_consolethread->launch();
		
		_netserverthread->launch();
		
		_rpggamethread->launch();
		
		_donemutex.lock();
		while (!_done)
		{
			_donemutex.unlock();
			
			sf::sleep(sf::seconds(1));
			
			_donemutex.lock();
		}
		_donemutex.unlock();
		
		std::cerr << "Application sleep loop done" << std::endl;
		
		std::cerr << "Terminating console connection thread" << std::endl;
		_consolethread->terminate();
		
		_netserver->finish();
		std::cerr << "Netserver finish called" << std::endl;
		_netserverthread->wait();
		std::cerr << "Netserver thread completed" << std::endl;
		
		_gameserver->finish();
		std::cerr << "Gameserver finish called" << std::endl;
		_gameserverthread->wait();
		std::cerr << "Gameserver thread completed" << std::endl;
		
		_rpggame->finish();
		_rpggamethread->wait();
    
		return 0;
	}
	
	void Application::finish()
	{
		std::cerr << "Finish called." << std::endl;
		sf::Lock donelock(_donemutex);
		std::cerr << "Done lock acquired." << std::endl;
		_done = true;
	}

	MySQL* Application::sql()
	{
		return _mysql;
	}
	
	RPGGame* Application::rpg()
	{
		return _rpggame;
	}
	
	CommandParser* Application::parser()
	{
		return _commandparser;
	}

	void Application::start_gameserver()
	{
		_gameserver->start();
	}
	
	void Application::start_netserver()
	{
		_netserver->start();
	}
}
