#include "application.h"
#include "netserver.h"
#include "gameserver.h"
#include "consoleconnection.h"
#include "mysql.h"
#include "rpgengine.h"
#include "commandparser.h"

#include <iostream>
#include <fstream>

namespace teh
{
	Application::Application()
		: _gameserver(0), _netserver(0), _rpgengine(0),
		_gameserverthread(0),
		_netserverthread(0),
		_rpgenginethread(0),
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
		if (_rpgengine)
			delete _rpgengine;
		if (_rpgenginethread)
			delete _rpgenginethread;
		if (_commandparser)
			delete _commandparser;
	}
	
	int Application::init(int argc, char** argv)
	{
		srand(100);
		
		po::options_description generic_opts("Allowed options");
		generic_opts.add_options()
			("port", po::value<unsigned short int>()->default_value(3137), "Port for server to listen on for client connections")
		;
		
		po::options_description cmdline_opts("Allowed command line options");
		cmdline_opts.add_options()
			("configfile,c", po::value<std::string>()->default_value("conf/tehrpg.conf"), "Config file path")
			("help", "Output help message")
		;
		
		po::options_description config_opts("Allowed configuration options");
		config_opts.add_options()
			("sqlhost", po::value<std::string>()->default_value("localhost"), "SQL server hostname")
			("sqldb", po::value<std::string>()->default_value("tehrpg"), "SQL database name")
			("sqluser", po::value<std::string>()->default_value("tehrpg"), "SQL database username")
			("sqlpass", po::value<std::string>(), "SQL database password")
			("sqlport", po::value<unsigned short int>()->default_value(3306), "SQL server port")
		;
		
		po::options_description real_cmdline_opts;
		real_cmdline_opts.add(generic_opts).add(cmdline_opts);
		
		po::options_description real_config_opts;
		real_config_opts.add(generic_opts).add(config_opts);
		
		po::options_description visible_config_opts;
		visible_config_opts.add(generic_opts).add(cmdline_opts).add(config_opts);

		store(po::command_line_parser(argc, argv).options(real_cmdline_opts).run(), _vm);
		notify(_vm);
		
		std::ifstream configifstream(_vm["configfile"].as<std::string>().c_str());
		if (!configifstream)
		{
			std::cerr << "Can not open config file " << _vm["configfile"].as<std::string>() << std::endl;
			return 2;
		}
		else
		{
			store(parse_config_file(configifstream, real_config_opts), _vm);
			notify(_vm);
		}
		
		if (_vm.count("help"))
		{
			std::cout << visible_config_opts;
			return -1;
		}
	
		//Done parsing arguments
		
		_gameserver = new GameServer(this);
		_gameserverthread = new sf::Thread(&Application::start_gameserver, this);
		_netserver = new NetServer(_vm["port"].as<unsigned short int>(), _gameserver);
		_netserverthread = new sf::Thread(&Application::start_netserver, this);
	
		_consoleconnection = new ConsoleConnection();
		_consolethread = new sf::Thread(&ConsoleConnection::start, _consoleconnection);

		_mysql = new MySQL(_vm["sqlhost"].as<std::string>(), _vm["sqlport"].as<unsigned short int>(), _vm["sqluser"].as<std::string>(), _vm["sqlpass"].as<std::string>(), _vm["sqldb"].as<std::string>());
	
		_rpgengine = new RPG::Engine(this, _gameserver);
		_rpgenginethread = new sf::Thread(&RPG::Engine::start, _rpgengine);
	
		_commandparser = new CommandParser();

		return 0;
	}
  
	int Application::start(int argc, char** argv)
	{
		int result = init(argc, argv);
		if (result != 0)
		{
			if (result < 0)
				return 0;
			return result;
		}

		_gameserverthread->launch();
		_gameserver->add_connection(_consoleconnection);
		_consolethread->launch();
		
		_netserverthread->launch();
		
		_rpgenginethread->launch();
		
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
		
		_rpgengine->finish();
		_rpgenginethread->wait();
    
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
	
	RPG::Engine* Application::rpg()
	{
		return _rpgengine;
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
