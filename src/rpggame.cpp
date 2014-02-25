#include "rpggame.h"
#include "application.h"
#include "gameserver.h"
#include "mysql.h"
#include "rpgcharacter.h"
#include "rpgtile.h"
#include "rpginventory.h"
#include "rpgitemtype.h"
#include "rpgiteminstance.h"
#include "rpgobject.h"
#include "rpgcommandhandler.h"
#include "rpgworld.h"
#include "rpgworldbuilder2.h"

namespace teh
{
namespace RPG
{
	Game::Game(Application* parent, GameServer* server)
		: _parent(parent), _server(server)
	{
		/*World world(1000, 1000, 1);
		MapTunnelerBuilder builder(1000, 25);
		world.build(std::bind(&MapTunnelerBuilder::build, std::ref(builder), std::placeholders::_1, std::placeholders::_2));
		world.savePNG("world.png");*/
	}
	
	Game::~Game()
	{
		for (std::map<unsigned int, Character*>::iterator i = _characters.begin(); i != _characters.end(); i++)
		{
			delete (*i).second;
		}
		for (std::map<unsigned int, Tile*>::iterator i = _tiles.begin(); i != _tiles.end(); i++)
		{
			delete (*i).second;
		}
		for (std::map<unsigned int, Inventory*>::iterator i = _inventories.begin(); i != _inventories.end(); i++)
		{
			delete (*i).second;
		}
		for (std::map<unsigned int, ItemType*>::iterator i = _itemtypes.begin(); i != _itemtypes.end(); i++)
		{
			delete (*i).second;
		}
		for (std::map<unsigned int, ItemInstance*>::iterator i = _iteminstances.begin(); i != _iteminstances.end(); i++)
		{
			delete (*i).second;
		}
	}
	
	
	void Game::init()
	{
		_parent->parser()->add_handler(new CommandHandler(this));
	}
	
	void Game::start()
	{
		init();
	}
	
	void Game::finish()
	{
		
	}
	
	clientid Game::check_logged_in(const std::string& charactername)
	{
		clientid remove = -1;
		for (std::map<clientid, Character*>::iterator i = _activecharacters.begin(); i != _activecharacters.end(); i++)
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
	
	clientid Game::check_logged_in(Character* character)
	{
		if (character)
			return check_logged_in(character->name());
		return -1;
	}
	
	void Game::logout(const clientid& client)
	{
		if (_activecharacters.count(client) == 0)
			return;
		
		Character* character = _activecharacters[client];
		_activecharacters.erase(client);
		
		Tile* room = character->get_location();
		
		room->broadcast(character->name() + " magically disappears!");
	}
			
	Character* Game::select_character(const clientid& client, const std::string& charactername)
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
			Character* character = get_character(charid);
			if (check_logged_in(character) == -1)
			{
				gc->state(GameClient::PlayingState);
				_activecharacters[client] = character;
				return character;
			}
		}
		
		return 0;
	}
	
	Character* Game::get_character(unsigned int id)
	{
		if (_characters.count(id) == 0)
		{
			sql::Connection* conn = sql()->connect();
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT * FROM `Characters` WHERE `id` = ?");
			prep_stmt->setUInt(1, id);
			sql::ResultSet* res = prep_stmt->executeQuery();
			if (res->rowsCount() == 1)
			{				
				_characters[id] = new Character(id, this);
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
	
	Character* Game::get_active_character(const clientid& client)
	{
		if (_activecharacters.count(client))
		{
			return _activecharacters[client];
		}
		return 0;
	}
	
	stringvector Game::character_names(const clientid& client)
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
	
	Tile* Game::get_tile(unsigned int id)
	{
		if (_tiles.count(id) == 0)
		{
			sql::Connection* conn = sql()->connect();
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT * FROM `Tiles` WHERE `id` = ?");
			prep_stmt->setUInt(1, id);
			sql::ResultSet* res = prep_stmt->executeQuery();
			if (res->rowsCount() == 1)
			{
				_tiles[id] = new Tile(id, this);
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
		return _tiles[id];
	}
	
	void Game::locate_tile(const long int& xpos, const long int& ypos, Tile* room)
	{
		_tilescoords[xpos][ypos] = room;
	}
	
	Tile* Game::find_tile(const long int& xpos, const long int& ypos)
	{
		if (!Game::valid_coord(xpos, ypos))
			return 0;
		
		if (_tilescoords.count(xpos))
		{
			if (_tilescoords[xpos].count(ypos))
			{
				return _tilescoords[xpos][ypos];
			}
		}
		
		sql::Connection* conn = sql()->connect();
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `id` FROM `Tiles` WHERE `xpos` = ? AND `ypos` = ?");
		prep_stmt->setInt(1, xpos);
		prep_stmt->setInt(2, ypos);
		sql::ResultSet* res = prep_stmt->executeQuery();
		if (res->rowsCount() == 1)
		{
			res->next();
			unsigned int id = res->getInt(1);
			
			delete res;
			delete prep_stmt;
			delete conn;
			
			return get_tile(id);
		}
		delete res;
		delete prep_stmt;
		delete conn;
		
		return 0;
	}
	
	Inventory* Game::get_inventory(unsigned int id)
	{
		if (_inventories.count(id) == 0)
		{
			sql::Connection* conn = sql()->connect();
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT * FROM `Inventories` WHERE `id` = ?");
			prep_stmt->setUInt(1, id);
			sql::ResultSet* res = prep_stmt->executeQuery();
			if (res->rowsCount() == 1)
			{
				_inventories[id] = new Inventory(id, this);
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
		return _inventories[id];
	}
	
	ItemType* Game::get_itemtype(unsigned int id)
	{
		if (_itemtypes.count(id) == 0)
		{
			sql::Connection* conn = sql()->connect();
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT * FROM `ItemTypes` WHERE `id` = ?");
			prep_stmt->setUInt(1, id);
			sql::ResultSet* res = prep_stmt->executeQuery();
			if (res->rowsCount() == 1)
			{
				_itemtypes[id] = new ItemType(id, this);
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
		return _itemtypes[id];
	}
	
	ItemType* Game::find_itemtype(const std::string& name)
	{
		sql::Connection* conn = sql()->connect();
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `id` FROM `ItemTypes` WHERE `name` = ?");
		prep_stmt->setString(1, name);
		sql::ResultSet* res = prep_stmt->executeQuery();
		if (res->rowsCount() == 1)
		{
			res->next();
			unsigned int id = res->getInt(1);
			
			delete res;
			delete prep_stmt;
			delete conn;
			
			return get_itemtype(id);
		}
		delete res;
		delete prep_stmt;
		delete conn;
		
		return 0;
	}
	
	ItemInstance* Game::get_iteminstance(unsigned int id)
	{
		if (_iteminstances.count(id) == 0)
		{
			sql::Connection* conn = sql()->connect();
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT * FROM `ItemInstances` WHERE `id` = ?");
			prep_stmt->setUInt(1, id);
			sql::ResultSet* res = prep_stmt->executeQuery();
			if (res->rowsCount() == 1)
			{
				_iteminstances[id] = new ItemInstance(id, this);
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
		return _iteminstances[id];
	}
	
	Object* Game::get_object(unsigned int objectid)
	{
		if (_objects.count(objectid) == 0)
		{
			sql::Connection* conn = sql()->connect();
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT * FROM `Object` WHERE `id` = ?");
			prep_stmt->setUInt(1, objectid);
			sql::ResultSet* res = prep_stmt->executeQuery();
			if (res->rowsCount() == 1)
			{
				_objects[objectid] = new Object(objectid, this);
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
		return _objects[objectid];
	}
	
	MySQL* Game::sql()
	{
		return _parent->sql();
	}
	
	void Game::message_client(const clientid& client, const std::string& message)
	{
		GameClient* gc = _server->get_client(client);
		if (gc)
			gc->write_line(message);
	}
	
	GameClient* Game::get_client(const clientid& client)
	{
		return _server->get_client(client);
	}
	
	bool Game::valid_coord(const long int& xpos, const long int& ypos)
	{
		if (xpos < 0)
		{
			if (xpos < -Game::WorldXSize)
				return false;
		}
		else
		{
			if (xpos >= Game::WorldXSize)
				return false;
		}
		if (ypos < 0)
		{
			if (ypos < -Game::WorldYSize)
				return false;
		}
		else
		{
			if (ypos >= Game::WorldYSize)
				return false;
		}
		return true;
	}
}
}