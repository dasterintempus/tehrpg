#include "gameserver.h"
#include <sstream>

#include "application.h"
#include "mysql.h"
#include "rpgengine.h"
#include "metagamecommandhandler.h"

namespace teh
{
	
	//
	// GameClient
	//
	
	GameClient::GameClient(GameConnectionInterface* conn)
		: _conn(conn), _userid(0), _state(WelcomeState), _permissions(UserPermissions)
	{
		generate_challenge();
	}
	
	void GameClient::generate_challenge()
	{
		char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		
		_challenge = "";
		
		unsigned int count = (rand() % 5) + 20;
		
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
	
	unsigned int GameClient::userid()
	{
		return _userid;
	}
	
	void GameClient::userid(const unsigned int& id)
	{
		_userid = id;
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
		delete _clients[0]; //cleanup root client
	}
	
	clientid GameServer::add_connection(GameConnectionInterface* conn)
	{
		sf::Lock clientslock(_clientsmutex);
		clientid id = 0;
		if (_next == 0) //Root client
		{
			//id remains 0
			_next++;
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
		{
			delete _clients[id];
			_clients.erase(id);
			_parent->rpg()->logout(id);
		}
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
		_parent->parser()->add_handler(new MetagameCommandHandler(this));
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
	
	clientid GameServer::find_from_userid(const unsigned int& id)
	{
		sf::Lock clientslock(_clientsmutex);
		for (std::map<clientid, GameClient*>::const_iterator i = _clients.begin();i != _clients.end(); i++)
		{
			if ((*i).second->userid() == id && (*i).second->state() != GameClient::LoginState)
				return (*i).first;
		}
		return -1;
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
	
	RPG::Engine* GameServer::rpg()
	{
		return _parent->rpg();
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
	
	void GameServer::update_permissions(GameClient* client)
	{
		client->permissions(_parent->sql()->get_permissions(client->userid()));
	}
	
	void GameServer::process_line(const clientid& id, const std::string& line)
	{
		if (_clients.count(id) == 0)
			return;
		
		_parent->parser()->parse(line, id);
	}
	
	std::string GameServer::greeting(const clientid& id)
	{
		std::stringstream strstream;
		strstream << "Welcome client #" << id;
		return strstream.str();
	}
	
}
