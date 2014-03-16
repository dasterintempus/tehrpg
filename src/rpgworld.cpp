#include "rpgworld.h"
#include "rpgengine.h"
#include "commandparser.h"

namespace teh
{
	namespace RPG
	{
		World::World(Engine* engine)
			: _engine(engine), _xsize(-1), _ysize(-1)
		{
			
		}
		
		void World::load()
		{
			sql::Connection* conn = _engine->sql()->connect();
			sql::Statement* stmt = conn->createStatement();
			
			sql::ResultSet* res = stmt->executeQuery("SELECT `value` FROM `etc` WHERE `key` = 'xsize.world'");
			if (res->rowsCount() == 0)
			{
				delete res;
				delete stmt;
				delete conn;
				return;
			}
			
			res->next();
			_xsize = to_numeric<long int>(res->getString(1));
			
			delete res;
			
			res = stmt->executeQuery("SELECT `value` FROM `etc` WHERE `key` = 'ysize.world'");
			if (res->rowsCount() == 0)
			{
				delete res;
				delete stmt;
				delete conn;
				return;
			}
			
			res->next();
			_ysize = to_numeric<long int>(res->getString(1));
			
			delete res;
			
			delete stmt;
			delete conn;
		}
		
		void World::init_size(long int xsize, long int ysize)
		{
			_xsize = xsize;
			_ysize = ysize;
		}
		
		long int World::xsize()
		{
			return _xsize;
		}
		
		long int World::ysize()
		{
			return _ysize;
		}
		
		/*
		void World::add_tile(long int x, long int y, const std::string& summary, const std::string& description, const std::string& terraintype)
		{
			if (x < 0 || y < 0 || x >= _xsize || y >= _ysize)
			{
				return;
			}
			
			_tiles[x][y].summary = summary;
			_tiles[x][y].description = description;
			_tiles[x][y].terraintype = terraintype;
			_tiles[x][y].open = true;
		}
		*/
		
		void World::set_map_value(const std::string& map, long int x, long int y, float val)
		{
			if (_mapvals.count(map) == 0)
			{
				_mapvals[map] = std::vector<std::vector<float> >();
				for (long int x = 0; x < _xsize; x++)
				{
					_mapvals[map].push_back(std::vector<float>());
					for (long int y = 0; y < _ysize; y++)
					{
						_mapvals[map][x].push_back(0);
					}
				}
			}
			_mapvals[map][x][y] = val;
		}
		
		float World::get_map_value(const std::string& map, long int x, long int y)
		{
			if (_mapvals.count(map) == 0)
			{
				return 0;
			}
			return _mapvals[map][x][y];
		}
		
		void World::set_map_flag(const std::string& map, long int x, long int y, const std::string& flag)
		{
			if (_mapflags.count(map) == 0)
			{
				_mapflags[map] = std::vector<std::vector<std::string> >();
				for (long int x = 0; x < _xsize; x++)
				{
					_mapflags[map].push_back(std::vector<std::string>());
					for (long int y = 0; y < _ysize; y++)
					{
						_mapflags[map][x].push_back("");
					}
				}
			}
			_mapflags[map][x][y] = flag;
		}
		
		std::string World::get_map_flag(const std::string& map, long int x, long int y)
		{
			if (_mapflags.count(map) == 0)
			{
				return "";
			}
			return _mapflags[map][x][y];
		}
		
		void World::save()
		{
			sql::Connection* conn = _engine->sql()->connect();
			std::vector<Value> bvalues;
			for (auto i = _mapvals.begin(); i != _mapvals.end(); i++)
			{
				Value value;
				std::string map = (*i).first;
				auto values = (*i).second;
				value.map = map;
				for (long int x = 0; x < _xsize;x++)
				{
					for (long int y = 0; y < _ysize;y++)
					{
						value.x = x;
						value.y = y;
						value.val = values[x][y];
						bvalues.push_back(value);
						if (bvalues.size() >= 256)
						{
							batchvalues(conn, bvalues);
							bvalues.clear();
						}
					}
				}
			}
			if (bvalues.size() > 0)
			{
				batchvalues(conn, bvalues);
				bvalues.clear();
			}
			
			std::vector<Flag> bflags;
			for (auto i = _mapflags.begin(); i != _mapflags.end(); i++)
			{
				Flag flag;
				std::string map = (*i).first;
				auto flags = (*i).second;
				flag.map = map;
				for (long int x = 0; x < _xsize;x++)
				{
					for (long int y = 0; y < _ysize;y++)
					{
						flag.x = x;
						flag.y = y;
						flag.flag = flags[x][y];
						bflags.push_back(flag);
						if (bflags.size() >= 256)
						{
							batchflags(conn, bflags);
							bflags.clear();
						}
					}
				}
			}
			if (bflags.size() > 0)
			{
				batchflags(conn, bflags);
				bflags.clear();
			}
			delete conn;
		}
		
		void World::batchvalues(sql::Connection* conn, const std::vector<World::Value>& values)
		{
			sql::Statement* stmt = conn->createStatement();
			
			std::stringstream querystr;
			querystr << "INSERT INTO `WorldValues` VALUES ";
			for (unsigned int n = 0;n < values.size();n++)
			{
				querystr << "('" << values[n].map << "', " << values[n].x << ", " << values[n].y << ", " << values[n].val << ")";
				if (n != values.size() -1)
				{
					querystr << ", ";
				}
			}
			
			stmt->execute(querystr.str().c_str());
		}
		
		void World::batchflags(sql::Connection* conn, const std::vector<World::Flag>& flags)
		{
			sql::Statement* stmt = conn->createStatement();
			
			std::stringstream querystr;
			querystr << "INSERT INTO `WorldFlags` VALUES ";
			for (unsigned int n = 0;n < flags.size();n++)
			{
				querystr << "('" << flags[n].map << "', " << flags[n].x << ", " << flags[n].y << ", '" << flags[n].flag << "')";
				if (n != flags.size() -1)
				{
					querystr << ", ";
				}
			}
			
			stmt->execute(querystr.str().c_str());
		}
		
		/*
		void World::batchtiles(sql::Connection* conn, const std::vector<std::pair<Vec2<int>, Tile> >& tiles)
		{
			sql::Statement* stmt = conn->createStatement();
		
			sql::ResultSet* res;
			
			//Make an entity
			//Build query
			std::stringstream querystr;
			querystr << "INSERT INTO `Entities` VALUES ";
			for (unsigned int n = 0;n < tiles.size();n++)
			{
				Vec2<int> v = tiles[n].first;
				Tile t = tiles[n].second;
				querystr << "(" << 1000 + v.x() + (v.y() * _xsize) << ", NULL, \"tile.map.entity\")";
				if (n != tiles.size() -1)
				{
					querystr << ", ";
				}
			}
			
			try
			{
				stmt->execute(querystr.str().c_str());
			}
			catch (sql::SQLException &e)
			{
				delete stmt;
				std::cerr << "SQL Error: " << e.what() << std::endl;
				return;
			}
			
			querystr.str("");
			
			//Description components
			
			querystr << "INSERT INTO `descriptionComponents` (`summary`, `description`, `entity_id`) VALUES ";
			for (unsigned int n = 0;n < tiles.size();n++)
			{
				Vec2<int> v = tiles[n].first;
				Tile t = tiles[n].second;
				
				querystr << "(\"" << t.summary << "\", \"" << t.description << "\", " << 1000 + v.x() + (v.y() * _xsize) << ")";
				if (n != tiles.size() -1)
				{
					querystr << ", ";
				}
			}
			
			try
			{
				stmt->execute(querystr.str().c_str());
			}
			catch (sql::SQLException &e)
			{
				delete stmt;
				std::cerr << "SQL Error: " << e.what() << std::endl;
				return;
			}
			
			querystr.str("");
			
			
			//Location components
			
			querystr << "INSERT INTO `locationComponents` (`xpos`, `ypos`, `entity_id`) VALUES ";
			for (unsigned int n = 0;n < tiles.size();n++)
			{
				Vec2<int> v = tiles[n].first;
				Tile t = tiles[n].second;
				
				querystr << "(" << v.x() << ", " << v.y() << ", " << 1000 + v.x() + (v.y() * _xsize) << ")";
				if (n != tiles.size() -1)
				{
					querystr << ", ";
				}
			}
			
			try
			{
				stmt->execute(querystr.str().c_str());
			}
			catch (sql::SQLException &e)
			{
				delete stmt;
				std::cerr << "SQL Error: " << e.what() << std::endl;
				return;
			}
			
			querystr.str("");
			
			//Location components
			
			querystr << "INSERT INTO `terrainComponents` (`type`, `health`, `entity_id`) VALUES ";
			for (unsigned int n = 0;n < tiles.size();n++)
			{
				Vec2<int> v = tiles[n].first;
				Tile t = tiles[n].second;
				
				querystr << "(\"" << t.terraintype << "\", " << 100 << ", " << 1000 + v.x() + (v.y() * _xsize) << ")";
				if (n != tiles.size() -1)
				{
					querystr << ", ";
				}
			}
			
			try
			{
				stmt->execute(querystr.str().c_str());
			}
			catch (sql::SQLException &e)
			{
				delete stmt;
				std::cerr << "SQL Error: " << e.what() << std::endl;
				return;
			}
			
			querystr.str("");
			
			delete stmt;
		}
		*/
		
		/*
		void World::setPixel(std::vector<unsigned char>& bytes, long int x, long int y, const Color& c)
		{
			int n = (4 * _xsize * y) + (4 * x);
			bytes[n] = c.r;
			bytes[n+1] = c.g;
			bytes[n+2] = c.b;
			bytes[n+3] = c.a;
		}
		*/
	}
}