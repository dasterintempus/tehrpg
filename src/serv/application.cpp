#include "application.h"
#include "netserver.h"
#include "gameserver.h"
#include "consoleconnection.h"

#include <iostream>

namespace teh
{
	Application::Application(int argc, char** argv)
		: _gameserver(0), _netserver(0),
		_gameserverthread(0),
		_netserverthread(0),
		_done(false),
		_consoleconnection(0),
		_mysql(0)
	{
		boost::program_options::options_description desc("Allowed Options");
		desc.add_options()
			("help", "produce help message")
			("verbosity,v", boost::program_options::value<unsigned short int>(), "Set verbosity")
			("port", boost::program_options::value<unsigned short int>()->default_value(3137), "Port to listen on")
		;
		
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), _cliargs);
		boost::program_options::notify(_cliargs);
		
		_gameserver = new GameServer(this);
		_gameserverthread = new sf::Thread(&Application::start_gameserver, this);
		_netserver = new NetServer(_cliargs["port"].as<unsigned short int>(), _gameserver);
		_netserverthread = new sf::Thread(&Application::start_netserver, this);
		
		_consoleconnection = new ConsoleConnection();
		_consolethread = new sf::Thread(&ConsoleConnection::start, _consoleconnection);

		_mysql = new MySQL("localhost", 3306, "tehmud", "Tur7tle$", "tehmud");
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
	}
	
	void Application::start()
	{
		_gameserverthread->launch();
		_gameserver->add_connection(_consoleconnection);
		_consolethread->launch();
		
		_netserverthread->launch();
		
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

	void Application::start_gameserver()
	{
		_gameserver->start();
	}
	
	void Application::start_netserver()
	{
		_netserver->start();
	}
}
