#include "gameserver.h"
#include <sstream>
#include "application.h"
#include "mysql.h"
#include "rpggame.h"
#include "rpgcharacter.h"
#include "gamecommandhandler.h"

namespace teh
{
	
	//
	// GameClient
	//
	
	GameClient::GameClient(GameConnectionInterface* conn)
		: _conn(conn), _state(WelcomeState), _permissions(UserPermissions)
	{
		generate_challenge();
	}
	
	void GameClient::generate_challenge()
	{
		char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		
		_challenge = "";
		
		unsigned int count = (rand() % 5) + 5;
		
		for (unsigned int n = 0;n < count;n++)
		{
			_challenge.append(1, alphanum[rand() % (sizeof(alphanum)-1)]);
		}
		std::cerr << _challenge << std::endl;
	}
			
	bool GameClient::has_line()
	{
		return _conn->has_line();
	}
	
	std::string GameClient::read_line()
	{
		return _conn->read_line();
	}
	
	void GameClient::write_line(const std::string& line)
	{
		_conn->write_line(line);
	}

	void GameClient::close()
	{
		state(GameClient::ClosingState);
		_conn->close();
	}

	GameClient::State GameClient::state()
	{
		return _state;
	}
	
	void GameClient::state(const GameClient::State& s)
	{
		_state = s;
	}
	
	std::string GameClient::username()
	{
		return _username;
	}
	
	void GameClient::username(const std::string& un)
	{
		_username = un;
	}
	
	unsigned short int GameClient::permissions()
	{
		return _permissions;
	}
	
	void GameClient::permissions(const unsigned short int& p)
	{
		_permissions = p;
	}
	
	std::string GameClient::challenge()
	{
		return _challenge;
	}
	
	//
	// GameServer
	//
	
	GameServer::GameServer(Application* parent)
		: _parent(parent), _next(0), _done(false)
	{
		
	}
	
	GameServer::~GameServer()
	{
		//NetServer handles cleanup of clients
	}
	
	clientid GameServer::add_connection(GameConnectionInterface* conn)
	{
		sf::Lock clientslock(_clientsmutex);
		clientid id = 0;
		if (_next == 0) //Root client
		{
			//id remains 0
			_clients[id] = new GameClient(conn);
			_clients[id]->write_line(greeting(id));
		}
		else
		{
			id = _next++;
			_clients[id] = new GameClient(conn);
			_clients[id]->write_line(greeting(id));
		}
		return id;
	}
	
	void GameServer::remove_client(const clientid& id)
	{
		sf::Lock clientslock(_clientsmutex);
		if (_clients.count(id))
			_clients.erase(id);
	}
	
	void GameServer::close_client(const clientid& id)
	{
		sf::Lock clientslock(_clientsmutex);
		if (_clients.count(id))
		{
			_clients[id]->close();
		}
	}
	
	void GameServer::init()
	{
		_parent->parser()->add_handler(new GameCommandHandler(this));
	}
	
	void GameServer::start()
	{
		init();
		
		_donemutex.lock();
		while (!_done)
		{
			_donemutex.unlock();
			
			for (std::map<clientid, GameClient*>::const_iterator i = _clients.begin();i != _clients.end(); i++)
			{
				clientid id = (*i).first;
				GameClient* client = (*i).second;
				if (client->has_line())
				{
					process_line(id, client->read_line());
				}
			}
			
			sf::sleep(sf::milliseconds(1));
			
			_donemutex.lock();
		}
		_donemutex.unlock();
		
		mysql_thread_end();
	}
	
	void GameServer::finish()
	{
		sf::Lock donelock(_donemutex);
		_done = true;
	}
	
	GameClient* GameServer::get_client(const clientid& id)
	{
		if (_clients.count(id))
			return _clients[id];
		return 0;
	}
	
	MySQL* GameServer::sql()
	{
		return _parent->sql();
	}

	bool GameServer::kill(const std::string& username, const std::string& killer)
	{
		clientid killid = find_from_username(username);
		return kill(killid, killer);
	}
	
	bool GameServer::kill(const clientid& id, const std::string& killer)
	{
		if (id != 0 && _clients.count(id))
		{
			_clients[id]->write_line("Disconnected by " + killer);
			_clients[id]->close();
			return true;
		}
		return false;
	}
	
	void GameServer::shutdown(const std::string username)
	{
		std::cerr << "Shutdown initiated by " << username << std::endl;
		_parent->finish();
	}
	
	void GameServer::process_line(const clientid& id, const std::string& line)
	{
		if (_clients.count(id) == 0)
			return;
		
		_parent->parser()->parse(line, id);
	}
	
	/*void GameServer::process_line(const clientid& id, const std::string& line)
	{
		if (_clients.count(id) == 0)
			return;
		
		GameClient* client = _clients[id];
		
		GameClient::State state = client->state();
		bool login = false;
		Command cmd = CommandLexer::lex(line, id);
		switch (state)
		{
			case GameClient::WelcomeState:
				if (cmd.slashed && cmd.arguments.size() > 0)
				{
					if (cmd.arguments[0] == "login")
					{
						client->write_line("Logging in!");
						client->write_line("Username: ");
						client->state(GameClient::UsernameState);
					}
					else if (cmd.arguments[0] == "register")
					{
						client->write_line("Registering!");
						client->write_line("Username: ");
						client->state(GameClient::RegisterUsernameState);
					}
					else if (cmd.arguments[0] == "login")
					{
						client->close();
					}
				}
				else
				{
					client->write_line("Invalid command: " + line);
				}
				break;
			case GameClient::UsernameState:
				client->username(line);
				client->write_line("Password: ");
				client->state(GameClient::PasswordState);
				break;
			case GameClient::PasswordState:
				login = try_login(client, line);
				if (login)
				{
					client->write_line("Logged in!");
					client->state(GameClient::LoggedInState);
					update_permissions(client);
				}
				else
				{
					client->write_line("Incorrect username/password");
					client->state(GameClient::WelcomeState);
				}
				break;
			case GameClient::LoggedInState:
				if (cmd.slashed && cmd.arguments.size() > 0)
				{
					if (client->permissions() & GameClient::RootPermissions)
					{
						client->write_line(process_root_command(client, cmd));
					}
					else if (client->permissions() & GameClient::ServerAdminPermissions)
					{
						client->write_line(process_admin_command(client, cmd));
					}
					else
					{
						client->write_line(process_user_command(client, cmd));
					}
				}
				else
				{
					client->write_line("Echo: " + line);
				}
				break;
			case GameClient::RegisterUsernameState:
				client->username(line);
				client->write_line("Password: ");
				client->state(GameClient::RegisterPasswordState);
				break;
			case GameClient::RegisterPasswordState:
				if (try_register(client, line))
				{
					client->write_line("Registered! (and logged in)");
					client->state(GameClient::LoggedInState);
					update_permissions(client);
				}
				else
				{
					client->write_line("Invalid or in-use Username");
					client->state(GameClient::WelcomeState);
				}
				break;
			case GameClient::PlayingState:
				if (cmd.slashed && cmd.arguments.size() > 0)
				{
					if (client->permissions() & GameClient::RootPermissions)
					{
						client->write_line(process_root_command(client, cmd));
					}
					else if (client->permissions() & GameClient::ServerAdminPermissions)
					{
						client->write_line(process_admin_command(client, cmd));
					}
					else
					{
						client->write_line(process_user_command(client, cmd));
					}
				}
				else
				{
					client->write_line("You say: " + line);
				}
			default:
				break;
		}
	}*/
	
	/*
	bool GameServer::try_register(GameClient* client, const std::string& password)
	{
		return _parent->sql()->register_user(client->username(), password, 1);
	}
	*/
	
	void GameServer::update_permissions(GameClient* client)
	{
		client->permissions(_parent->sql()->get_permissions(client->username()));
	}
	
	/*
	std::string GameServer::process_root_command(GameClient* client, const Command& cmd)
	{
		if (cmd.arguments[0] == "shutdown")
		{
			_parent->finish();
			return "Shutting down";
		}
		return process_admin_command(client, cmd);
	}
	
	std::string GameServer::process_admin_command(GameClient* client, const Command& cmd)
	{
		if (cmd.arguments[0] == "kill")
		{
			if (cmd.arguments.size() != 3)
			{
				return "Invalid number of arguments for kill command.\nUsage: /kill {id|user} {target}";
			}
			if (cmd.arguments[1] == "id")
			{
				std::stringstream conv;
				conv << cmd.arguments[2];
				clientid killid;
				conv >> killid;
				if (killid != 0 && _clients.count(killid))
				{
					_clients[killid]->close();
					return "Killed #" + conv.str();
				}
				return "Couldn't find #" + conv.str();
			}
			else if (cmd.arguments[1] == "user")
			{
				std::string username = cmd.arguments[2];
				clientid killid = find_from_username(username);
				if (killid != 0 && _clients.count(killid))
				{
					_clients[killid]->close();
					return "Killed " + username;
				}
				return "Couldn't find " + username;
			}
			else
			{
				return "Invalid argument for kill command.\nUsage: /kill {id|user} {target}";
			}
		}
		return process_user_command(client, cmd);
	}
	
	std::string GameServer::process_user_command(GameClient* client, const Command& cmd)
	{
		if (cmd.arguments[0] == "logout")
		{
			client->state(GameClient::WelcomeState);
			return "Logged out.";
		}
		else if (cmd.arguments[0] == "select")
		{
			if (cmd.arguments.size() != 2)
			{
				return "Incorrect number of commands for select command.\nUsage: /select {charactername}";
			}
			std::string charactername = cmd.arguments[1];
			if (_parent->rpg()->select_character(find_clientid(client), client->username(), charactername))
			{
				client->state(GameClient::PlayingState);
				return "Now Playing: " + charactername;
			}
		}
		return "Unknown command";
	}
	*/
	
	std::string GameServer::greeting(const clientid& id)
	{
		std::stringstream strstream;
		strstream << "Welcome client #" << id;
		return strstream.str();
	}
	
	clientid GameServer::find_clientid(GameClient* client)
	{
		sf::Lock clientslock(_clientsmutex);
		for (std::map<clientid, GameClient*>::const_iterator i = _clients.begin();i != _clients.end(); i++)
		{
			if ((*i).second == client)
				return (*i).first;
		}
		return -1;
	}
	
	clientid GameServer::find_from_username(const std::string& username)
	{
		sf::Lock clientslock(_clientsmutex);
		for (std::map<clientid, GameClient*>::const_iterator i = _clients.begin();i != _clients.end(); i++)
		{
			if ((*i).second->username() == username && (*i).second->state() != GameClient::LoginState)
				return (*i).first;
		}
		return -1;
	}
	
}
