#include "gameserver.h"
#include <sstream>
#include "application.h"
#include <mysql.h>

namespace teh
{
	
	//
	// GameClient
	//
	
	GameClient::GameClient(GameConnectionInterface* conn)
		: _conn(conn), _state(WelcomeState), _permissions(UserPermissions)
	{
		
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
	
	GameClient::Permissions GameClient::permissions()
	{
		return _permissions;
	}
	
	void GameClient::permissions(const GameClient::Permissions& p)
	{
		_permissions = p;
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
		clientid id = _next++;
		_clients[id] = new GameClient(conn);
		_clients[id]->write_line(greeting(id));
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
	
	void GameServer::start()
	{
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
	
	void GameServer::process_line(const clientid& id, const std::string& line)
	{
		if (_clients.count(id) == 0)
			return;
		
		GameClient* client = _clients[id];
		
		GameClient::State state = client->state();
		bool login = false;
		switch (state)
		{
			case GameClient::WelcomeState:
				std::cerr << "Gameserver sees: " << line << std::endl;
				std::cerr << "Line size: " << line.size() << std::endl;
				if (line == "/login")
				{
					client->write_line("Username: ");
					client->state(GameClient::UsernameState);
				}
				else if (line == "/register")
				{
					client->write_line("Username: ");
					client->state(GameClient::RegisterUsernameState);
				}
				else if (line == "/quit")
				{
					client->close();
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
				if (line[0] == '/')
				{
					if (client->permissions() == GameClient::RootPermissions)
					{
						client->write_line(process_root_command(client, line));
					}
					else if (client->permissions() == GameClient::AdminPermissions)
					{
						client->write_line(process_admin_command(client, line));
					}
					else
					{
						client->write_line(process_user_command(client, line));
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
					client->write_line("Registered!");
					client->state(GameClient::LoggedInState);
					update_permissions(client);
				}
				else
				{
					client->write_line("Invalid or in-use Username");
					client->state(GameClient::WelcomeState);
				}
				break;
			default:
				break;
		}
	}
	
	bool GameServer::try_login(GameClient* client, const std::string& password)
	{
		return _parent->sql()->validate_login(client->username(), password);
	}
	
	bool GameServer::try_register(GameClient* client, const std::string& password)
	{
		return _parent->sql()->register_user(client->username(), password, 1);
	}
	
	void GameServer::update_permissions(GameClient* client)
	{
		if (find_clientid(client) == 0)
		{
			client->permissions(GameClient::RootPermissions);
		}
		else if (client->username() == "dasterin")
		{
			client->permissions(GameClient::AdminPermissions);
		}
		else
		{
			client->permissions(GameClient::UserPermissions);
		}
	}

	std::string GameServer::process_root_command(GameClient* client, const std::string& line)
	{
		if (line == "/shutdown")
		{
			_parent->finish();
			return "Shutting down";
		}
		return process_admin_command(client, line);
	}
	
	std::string GameServer::process_admin_command(GameClient* client, const std::string& line)
	{
		if (line.compare(0, 6, "/kill ") == 0)
		{
			std::stringstream conv;
			conv << line.substr(6);
			clientid killid;
			conv >> killid;
			if (killid != 0 && _clients.count(killid))
			{
				_clients[killid]->close();
				return "Killed #" + conv.str();
			}
			return "Couldn't find #" + conv.str();
		}
		return process_user_command(client, line);
	}
	
	std::string GameServer::process_user_command(GameClient* client, const std::string& line)
	{
		if (line == "/logout")
		{
			client->state(GameClient::WelcomeState);
			return "Logged out.";
		}
		return "Unknown command";
	}
	
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
	
}
