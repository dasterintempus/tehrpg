#include "rpgtile.h"
#include "rpggame.h"
#include "rpgcharacter.h"
#include "rpginventory.h"
#include "mysql.h"

namespace teh
{
	const std::string RPGTile::northdir = std::string("north");
	const std::string RPGTile::southdir = std::string("south");
	const std::string RPGTile::eastdir = std::string("east");
	const std::string RPGTile::westdir = std::string("west");
	
	RPGTile* RPGTile::build(RPGGame* parent, const long int& xpos, const long int& ypos, bool solid, const std::string& description)
	{
		if (!RPGGame::valid_coord(xpos, ypos))
		{
			return 0;
		}
		
		sql::Connection* conn = parent->sql()->connect();
		sql::PreparedStatement* prep_stmt = 0;
		
		if (description != "")
		{
			prep_stmt = conn->prepareStatement("INSERT INTO `Tiles` VALUES (NULL, ?, ?, ?, ?)");
			prep_stmt->setInt(1, xpos);
			prep_stmt->setInt(2, ypos);
			prep_stmt->setBoolean(3, solid);
			prep_stmt->setString(4, description);
		}
		else
		{
			prep_stmt = conn->prepareStatement("INSERT INTO `Tiles` VALUES (NULL, ?, ?, ?)");
			prep_stmt->setInt(1, xpos);
			prep_stmt->setInt(2, ypos);
			prep_stmt->setBoolean(3, solid);
		}
		
		try
		{
			prep_stmt->execute();
		}
		catch (sql::SQLException &e)
		{
			delete prep_stmt;
			delete conn;
			return 0;
		}
		
		delete prep_stmt;
		
		prep_stmt = conn->prepareStatement("SELECT LAST_INSERT_ID()");
		sql::ResultSet* res = prep_stmt->executeQuery();
		res->next();
		unsigned int id = res->getUInt(1);
		delete res;
		delete prep_stmt;
		delete conn;
		
		RPGTile* tile = parent->get_tile(id);
		
		RPGInventory* inventory = RPGInventory::build(parent, tile);
		
		return tile;
	}
	
	RPGTile::RPGTile(unsigned int id, RPGGame* parent)
		: _id(id), _parent(parent)
	{
		sql::Connection* conn = _parent->sql()->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `description`, `xpos`, `ypos`, `solid` FROM `Tiles` WHERE `id` = ?");
		prep_stmt->setUInt(1, _id);
		sql::ResultSet* res = prep_stmt->executeQuery();
		res->next();
		_description = res->getString("description");
		_xpos = res->getInt("xpos");
		_ypos = res->getInt("ypos");
		_solid = res->getBoolean("solid");
		
		delete res;
		delete prep_stmt;
		delete conn;
		
		locate();
	}
	
	std::vector<RPGCharacter*> RPGTile::get_occupants(bool loggedin)
	{
		std::vector<RPGCharacter*> occupants;
		
		sql::Connection* conn = _parent->sql()->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `Characters`.`id` FROM `Tiles` JOIN `Characters` WHERE `Characters`.`tile_id` = `Tiles`.`id` AND `Tiles`.`id` = ?");
		prep_stmt->setInt(1, _id);
		sql::ResultSet* res = prep_stmt->executeQuery();
		while (res->next())
		{
			RPGCharacter* character = _parent->get_character(res->getInt(1));
			if (_parent->check_logged_in(character) != -1 || !loggedin)
				occupants.push_back(character);
		}
		delete res;
		delete prep_stmt;
		delete conn;
		
		return occupants;
	}
	
	void RPGTile::broadcast(const std::string& msg)
	{
		std::vector<RPGCharacter*> occupants = get_occupants();
		for (unsigned int n = 0;n < occupants.size();n++)
		{
			clientid client = _parent->check_logged_in(occupants[n]);
			if (client != -1)
			{
				_parent->message_client(client, msg);
			}
		}
	}
	
	unsigned int RPGTile::id()
	{
		return _id;
	}
	
	long int RPGTile::xpos()
	{
		return _xpos;
	}
	
	long int RPGTile::ypos()
	{
		return _ypos;
	}
	
	bool RPGTile::solid()
	{
		return _solid;
	}
	
	std::string RPGTile::description()
	{
		return _description;
	}
	
	bool RPGTile::can_exit_north()
	{
		return can_exit(0, -1);
	}
	
	bool RPGTile::can_exit_south()
	{
		return can_exit(0, 1);
	}
	
	bool RPGTile::can_exit_east()
	{
		return can_exit(-1, 0);
	}
	
	bool RPGTile::can_exit_west()
	{
		return can_exit(1, 0);
	}
	
	stringvector RPGTile::get_wall_sides()
	{
		stringvector wallsides;
		
		sql::Connection* conn = _parent->sql()->connect();
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `Walls`.`direction` FROM `Walls` WHERE `Walls`.`tile_id` = ?");
		prep_stmt->setInt(1, id());
		sql::ResultSet* res = prep_stmt->executeQuery();
		while (res->next())
		{
			wallsides.push_back(res->getString(1));
		}
		delete res;
		delete prep_stmt;
		
		RPGTile* other = _parent->find_tile(xpos()+1, ypos());
		if (other)
		{
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `Walls`.`direction` FROM `Walls` WHERE `Walls`.`tile_id` = ?");
			prep_stmt->setInt(1, other->id());
			sql::ResultSet* res = prep_stmt->executeQuery();
			while (res->next())
			{
				if (res->getString(1) == "west")
					wallsides.push_back("east");
			}
			delete res;
			delete prep_stmt;
		}
		
		other = _parent->find_tile(xpos(), ypos()+1);
		if (other)
		{
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `Walls`.`direction` FROM `Walls` WHERE `Walls`.`tile_id` = ?");
			prep_stmt->setInt(1, other->id());
			sql::ResultSet* res = prep_stmt->executeQuery();
			while (res->next())
			{
				if (res->getString(1) == "north")
					wallsides.push_back("south");
			}
			delete res;
			delete prep_stmt;
		}
		
		delete conn;
		
		return wallsides;
	}
	
	stringvector RPGTile::get_exits()
	{
		stringvector out;
		
		if (can_exit(1, 0))
			out.push_back("west");
		if (can_exit(-1, 0))
			out.push_back("east");
		if (can_exit(0, -1))
			out.push_back("north");
		if (can_exit(0, 1))
			out.push_back("south");
		
		return out;
	}
	
	RPGTile* RPGTile::can_exit(const int& dx, const int& dy)
	{
		if (abs(dx) > 1 || abs(dy) > 1)
			return 0;
		if (dx == 0 && dy == 0)
			return 0;
		
		RPGTile* other = _parent->find_tile(xpos()+dx, ypos()+dy);
		if (!other)
			return 0;
		if (other->solid())
			return 0;
		
		stringvector wallsides = get_wall_sides();
		for (unsigned int n = 0;n < wallsides.size();n++)
		{
			std::string side = wallsides[n];
			if (side == "north" and dy < 0)
				return 0;
			if (side == "south" and dy > 0)
				return 0;
			if (side == "west" and dx < 0)
				return 0;
			if (side == "east" and dx > 0)
				return 0;
		}
		return other;
	}
	
	RPGTile* RPGTile::can_exit(const std::string& direction)
	{
		if (direction == northdir.substr(0, direction.size()))
			return can_exit(0, -1);
		else if (direction == southdir.substr(0, direction.size()))
			return can_exit(0, 1);
		else if (direction == eastdir.substr(0, direction.size()))
			return can_exit(-1, 0);
		else if (direction == westdir.substr(0, direction.size()))
			return can_exit(1, 0);
		
		return 0;
	}
	
	RPGInventory* RPGTile::get_inventory()
	{
		sql::Connection* conn = _parent->sql()->connect();
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `id` FROM `Inventories` WHERE `tile_id` = ?");
		prep_stmt->setInt(1, id());
		sql::ResultSet* res = prep_stmt->executeQuery();
		res->next();
		
		RPGInventory* inventory = _parent->get_inventory(res->getUInt(1));
		
		delete res;
		delete prep_stmt;
		delete conn;
		
		return inventory;
	}
	
	void RPGTile::locate()
	{
		sql::Connection* conn = _parent->sql()->connect();
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `xpos`, `ypos` FROM `Tiles` WHERE `id` = ?");
		prep_stmt->setInt(1, id());
		sql::ResultSet* res = prep_stmt->executeQuery();
		res->next();
		
		_parent->locate_tile(res->getInt("xpos"),res->getInt("ypos"), this);
		
		delete res;
		delete prep_stmt;
		delete conn;
	}
}