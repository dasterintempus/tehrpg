#include "rpggame.h"
#include "application.h"
#include "gameserver.h"
#include "mysql.h"
#include "rpgcharacter.h"
#include "rpgroom.h"
#include "rpgcommandhandler.h"

namespace teh
{
	RPGGame::RPGGame(Application* parent, GameServer* server)
		: _parent(parent), _server(server)
	{
		
	}
	
	RPGGame::~RPGGame()
	{
		for (std::map<int, RPGCharacter*>::iterator i = _characters.begin(); i != _characters.end(); i++)
		{
			delete (*i).second;
		}
		for (std::map<int, RPGRoom*>::iterator i = _rooms.begin(); i != _rooms.end(); i++)
		{
			delete (*i).second;
		}
	}
	
	
	void RPGGame::init()
	{
		_parent->parser()->add_handler(new RPGCommandHandler(this));
	}
	
	void RPGGame::start()
	{
		init();
	}
	
	void RPGGame::finish()
	{
		
	}
	
	clientid RPGGame::check_logged_in(RPGCharacter* character)
	{
		for (std::map<clientid, RPGCharacter*>::iterator i = _activecharacters.begin(); i != _activecharacters.end(); i++)
		{
			if (character == (*i).second)
				return (*i).first;
		}
		return -1;
	}
			
	RPGCharacter* RPGGame::select_character(const clientid& client, const std::string& charactername)
	{
		GameClient* gc = _server->get_client(client);
		if (!gc)
			return 0;
		
		std::string username = gc->username();
		
		sql::Connection* conn = sql()->connect();
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `Characters`.`id` FROM `Characters` JOIN `Users` WHERE `Characters`.`user_id` = `Users`.`id` AND `Characters`.`name` = ? AND `Users`.`username` = ?");
		prep_stmt->setString(1, charactername.c_str());
		prep_stmt->setString(2, username.c_str());
		sql::ResultSet* res = prep_stmt->executeQuery();
		
		int charid = -1;
		if (res->rowsCount() > 0)
		{
			res->next();
			charid = res->getInt(1);
		}
		delete res;
		delete prep_stmt;
		delete conn;
		
		if (charid != -1)
		{
			gc.state(GameClient::PlayingState);
			RPGCharacter* character = get_character(charid);
			_activecharacters[client] = character;
			return character;
		}
		
		return 0;
	}
	
	RPGCharacter* RPGGame::get_character(int id)
	{
		if (_characters.count(id) == 0)
		{
			sql::Connection* conn = sql()->connect();
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT * FROM `Characters` WHERE `id` = ?");
			prep_stmt->setInt(1, id);
			sql::ResultSet* res = prep_stmt->executeQuery();
			if (res->rowsCount() == 1)
			{				
				_characters[id] = new RPGCharacter(id, this);
			}
			else
			{
				return 0;
			}
		}
		return _characters[id];
	}
	
	RPGCharacter* RPGGame::get_active_character(const clientid& client)
	{
		if (_activecharacters.count(client))
		{
			return _activecharacters[client];
		}
		return 0;
	}
	
	RPGRoom* RPGGame::get_room(int id)
	{
		if (_rooms.count(id) == 0)
		{
			sql::Connection* conn = sql()->connect();
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT * FROM `Rooms` WHERE `id` = ?");
			prep_stmt->setInt(1, id);
			sql::ResultSet* res = prep_stmt->executeQuery();
			if (res->rowsCount() == 1)
			{				
				_rooms[id] = new RPGRoom(id, this);
			}
			else
			{
				return 0;
			}
		}
		return _rooms[id];
	}
	
	void RPGGame::locate_room(long int x, long int y, short int z, RPGRoom* room)
	{
		_roomscoords[x][y][z] = room;
	}
	
	MySQL* RPGGame::sql()
	{
		return _parent->sql();
	}
	
	void RPGGame::message_client(const clientid& client, const std::string& message)
	{
		GameClient* gc = _server->get_client(client);
		if (gc)
			gc->write_line(message);
	}
}