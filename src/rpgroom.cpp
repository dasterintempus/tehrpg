#include "rpgroom.h"
#include "rpggame.h"
#include "rpgcharacter.h"
#include "mysql.h"

namespace teh
{
	RPGRoom* RPGRoom::build(RPGGame* parent, const long int& xpos, const long int& ypos, const short int& zpos, const std::string& description)
	{
		if (!RPGGame::valid_coord(xpos, ypos, zpos))
		{
			return 0;
		}
		
		sql::Connection* conn = parent->sql()->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("INSERT INTO `Rooms` VALUES (NULL, ?, ?, ?, ?)");
		prep_stmt->setInt(1, xpos);
		prep_stmt->setInt(2, ypos);
		prep_stmt->setInt(3, zpos);
		prep_stmt->setString(4, description);
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
		
		return new RPGRoom(id, parent);
	}
	
	RPGRoom::RPGRoom(unsigned int id, RPGGame* parent)
		: _id(id), _parent(parent)
	{
		sql::Connection* conn = _parent->sql()->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `description`, `xpos`, `ypos`, `zpos` FROM `Rooms` WHERE `id` = ?");
		prep_stmt->setUInt(1, _id);
		sql::ResultSet* res = prep_stmt->executeQuery();
		res->next();
		_description = res->getString("description");
		_xpos = res->getInt("xpos");
		_ypos = res->getInt("ypos");
		_zpos = res->getInt("zpos");
		delete res;
		delete prep_stmt;
		delete conn;
		
		locate();
	}
	
	std::vector<RPGCharacter*> RPGRoom::get_occupants()
	{
		std::vector<RPGCharacter*> occupants;
		
		sql::Connection* conn = _parent->sql()->connect();
		
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `Characters`.`id` FROM `Rooms` JOIN `Characters` WHERE `Characters`.`room_id` = `Rooms`.`id` AND `Rooms`.`id` = ?");
		prep_stmt->setInt(1, _id);
		sql::ResultSet* res = prep_stmt->executeQuery();
		while (res->next())
		{
			RPGCharacter* character = _parent->get_character(res->getInt(1));
			if (_parent->check_logged_in(character) != -1)
				occupants.push_back(character);
		}
		delete res;
		delete prep_stmt;
		delete conn;
		
		return occupants;
	}
	
	void RPGRoom::broadcast(const std::string& msg)
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
	
	unsigned int RPGRoom::id()
	{
		return _id;
	}
	
	long int RPGRoom::xpos()
	{
		return _xpos;
	}
	
	long int RPGRoom::ypos()
	{
		return _ypos;
	}
	
	short int RPGRoom::zpos()
	{
		return _zpos;
	}
	
	std::string RPGRoom::description()
	{
		return _description;
	}
	
	bool RPGRoom::can_exit_north()
	{
		return can_exit(0, -1, 0);
	}
	
	bool RPGRoom::can_exit_south()
	{
		return can_exit(0, 1, 0);
	}
	
	bool RPGRoom::can_exit_east()
	{
		return can_exit(-1, 0, 0);
	}
	
	bool RPGRoom::can_exit_west()
	{
		return can_exit(1, 0, 0);
	}
	
	bool RPGRoom::can_exit_up()
	{
		return can_exit(0, 0, -1);
	}
	
	bool RPGRoom::can_exit_down()
	{
		return can_exit(0, 0, 1);	
	}
	
	stringvector RPGRoom::get_wall_sides()
	{
		stringvector wallsides;
		sql::Connection* conn = _parent->sql()->connect();
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `Walls`.`side` FROM `Walls` WHERE `Walls`.`room_id` = ?");
		prep_stmt->setInt(1, id());
		sql::ResultSet* res = prep_stmt->executeQuery();
		while (res->next())
		{
			wallsides.push_back(res->getString(1));
		}
		delete res;
		delete prep_stmt;
		
		RPGRoom* other = _parent->find_room(xpos()+1, ypos(), zpos());
		if (other)
		{
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `Walls`.`side` FROM `Walls` WHERE `Walls`.`room_id` = ?");
			prep_stmt->setInt(1, other->id());
			sql::ResultSet* res = prep_stmt->executeQuery();
			while (res->next())
			{
				if (res->getString(1) == "east")
					wallsides.push_back("west");
			}
			delete res;
			delete prep_stmt;
		}
		
		other = _parent->find_room(xpos(), ypos()+1, zpos());
		if (other)
		{
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `Walls`.`side` FROM `Walls` WHERE `Walls`.`room_id` = ?");
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
		
		other = _parent->find_room(xpos(), ypos(), zpos()+1);
		if (other)
		{
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `Walls`.`side` FROM `Walls` WHERE `Walls`.`room_id` = ?");
			prep_stmt->setInt(1, other->id());
			sql::ResultSet* res = prep_stmt->executeQuery();
			while (res->next())
			{
				if (res->getString(1) == "up")
					wallsides.push_back("down");
			}
			delete res;
			delete prep_stmt;
		}
		return wallsides;
	}
	
	stringvector RPGRoom::get_exits()
	{
		stringvector out;
		
		if (can_exit(-1, 0, 0))
			out.push_back("east");
		if (can_exit(1, 0, 0))
			out.push_back("west");
		if (can_exit(0, -1, 0))
			out.push_back("north");
		if (can_exit(0, 1, 0))
			out.push_back("south");
		if (can_exit(0, 0, -1))
			out.push_back("up");
		if (can_exit(0, 0, 1))
			out.push_back("down");
		
		return out;
	}
	
	bool RPGRoom::can_exit(const int& dx, const int& dy, const int& dz)
	{
		RPGRoom* other = _parent->find_room(xpos()+dx, ypos()+dy, zpos()+dz);
		if (!other)
			return false;
		
		stringvector wallsides = get_wall_sides();
		for (unsigned int n = 0;n < wallsides.size();n++)
		{
			std::string side = wallsides[n];
			if (side == "north" and dy < 0)
				return false;
			if (side == "south" and dy > 0)
				return false;
			if (side == "east" and dx < 0)
				return false;
			if (side == "west" and dx > 0)
				return false;
			if (side == "up" and dz < 0)
				return false;
			if (side == "down" and dz > 0)
				return false;
		}
		return true;
	}
	
	void RPGRoom::locate()
	{
		sql::Connection* conn = _parent->sql()->connect();
		sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `xpos`, `ypos`, `zpos` FROM `Rooms` WHERE `id` = ?");
		prep_stmt->setInt(1, _id);
		sql::ResultSet* res = prep_stmt->executeQuery();
		res->next();
		
		_parent->locate_room(res->getInt("xpos"),res->getInt("ypos"),res->getInt("zpos"), this);
		
		delete res;
		delete prep_stmt;
		delete conn;
	}
}