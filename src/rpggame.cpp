#include "rpggame.h"
#include "application.h"
#include "gameserver.h"
#include "mysql.h"
#include "rpgcharacter.h"
#include "rpgroom.h"
#include "rpgcommandhandler.h"
#include "rpgworld.h"
#include "rpgworldbuilder2.h"

namespace teh
{
	RPGGame::RPGGame(Application* parent, GameServer* server)
		: _parent(parent), _server(server)
	{
		RPGWorld world(1000, 1000, 1);
		MapTunnelerBuilder builder(1000, 25);
		world.build(std::bind(&MapTunnelerBuilder::build, std::ref(builder), std::placeholders::_1, std::placeholders::_2));
		world.savePNG("world.png");
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
	
	clientid RPGGame::check_logged_in(const std::string& charactername)
	{
		clientid remove = -1;
		for (std::map<clientid, RPGCharacter*>::iterator i = _activecharacters.begin(); i != _activecharacters.end(); i++)
		{
			if (charactername == (*i).second->name())
			{
				GameClient* gc = _server->get_client((*i).first);
				if (gc)
				{
					return (*i).first;
				}
				else
				{
					remove = (*i).first;
					break;
				}
			}
		}
		if (remove != -1)
		{
			logout(remove);
		}
		return -1;
	}
	
	clientid RPGGame::check_logged_in(RPGCharacter* character)
	{
		if (character)
			return check_logged_in(character->name());
		return -1;
	}
	
	void RPGGame::logout(const clientid& client)
	{
		if (_activecharacters.count(client) == 0)
			return;
		
		RPGCharacter* character = _activecharacters[client];
		_activecharacters.erase(client);
		
		RPGRoom* room = character->get_location();
		
		room->broadcast(character->name() + " magically disappears!");
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
			RPGCharacter* character = get_character(charid);
			if (check_logged_in(character) == -1)
			{
				gc->state(GameClient::PlayingState);
				_activecharacters[client] = character;
				return character;
			}
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
				delete res;
				delete prep_stmt;
				delete conn;
				return 0;
			}			
			delete res;
			delete prep_stmt;
			delete conn;
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
	
	stringvector RPGGame::character_names(const clientid& client)
	{
		stringvector out;
		GameClient* gc = _server->get_client(client);
		if (!gc)
			return out;
		
		std::string username = gc->username();
		
		sql::Connection* conn = sql()->connect();
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `Characters`.`name` FROM `Characters` JOIN `Users` WHERE `Characters`.`user_id` = `Users`.`id` AND `Users`.`username` = ?");
		prep_stmt->setString(1, username.c_str());
		sql::ResultSet* res = prep_stmt->executeQuery();
		
		while (res->next())
		{
			out.push_back(res->getString(1));
		}
		delete res;
		delete prep_stmt;
		delete conn;
		
		return out;
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
				delete res;
				delete prep_stmt;
				delete conn;
				return 0;
			}
			delete res;
			delete prep_stmt;
			delete conn;
		}
		return _rooms[id];
	}
	
	void RPGGame::locate_room(const long int& xpos, const long int& ypos, const short int& zpos, RPGRoom* room)
	{
		_roomscoords[xpos][ypos][zpos] = room;
	}
	
	RPGRoom* RPGGame::find_room(const long int& xpos, const long int& ypos, const short int& zpos)
	{
		if (!RPGGame::valid_coord(xpos, ypos, zpos))
			return 0;
		
		if (_roomscoords.count(xpos))
		{
			if (_roomscoords[xpos].count(ypos))
			{
				if (_roomscoords[xpos][ypos].count(zpos))
				{
					return _roomscoords[xpos][ypos][zpos];
				}
			}
		}
		
		sql::Connection* conn = sql()->connect();
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `id` FROM `Rooms` WHERE `xpos` = ? AND `ypos` = ? AND `zpos` = ?");
		prep_stmt->setInt(1, xpos);
		prep_stmt->setInt(2, ypos);
		prep_stmt->setInt(3, zpos);
		sql::ResultSet* res = prep_stmt->executeQuery();
		if (res->rowsCount() == 1)
		{
			res->next();
			unsigned int id = res->getInt(1);
			return get_room(id);
		}
		return 0;
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
	
	GameClient* RPGGame::get_client(const clientid& client)
	{
		return _server->get_client(client);
	}
	
	bool RPGGame::valid_coord(const long int& xpos, const long int& ypos, const short int& zpos)
	{
		if (xpos < 0)
		{
			if (xpos < -RPGGame::WorldXSize)
				return false;
		}
		else
		{
			if (xpos >= RPGGame::WorldXSize)
				return false;
		}
		if (ypos < 0)
		{
			if (ypos < -RPGGame::WorldYSize)
				return false;
		}
		else
		{
			if (ypos >= RPGGame::WorldYSize)
				return false;
		}
		if (zpos < 0)
		{
			if (zpos < -RPGGame::WorldZSize)
				return false;
		}
		else
		{
			if (zpos >= RPGGame::WorldZSize)
				return false;
		}
		return true;
	}
}