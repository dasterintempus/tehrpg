#pragma once

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <utility>
#include "lodepng.h"
#include "vec2.h"
#include "mysql.h"

namespace teh
{
	namespace RPG
	{
		class Engine;
		
		/*
		struct Tile
		{
			std::string summary;
			std::string description;
			std::string terraintype;
			bool open;
		};
		*/
		
		/*
		struct Color
		{
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		};
		*/
		
		class World
		{
			public:
				struct Value
				{
					std::string map;
					long int x;
					long int y;
					float val;
				};
				struct Flag
				{
					std::string map;
					long int x;
					long int y;
					std::string flag;
				};
			
				World(Engine* engine);
				void load();
				void init_size(long int xsize, long int ysize);
				long int xsize();
				long int ysize();
				//void add_tile(long int x, long int y, const std::string& summary, const std::string& description, const std::string& terraintype);
				void set_map_value(const std::string& map, long int x, long int y, float val);
				float get_map_value(const std::string& map, long int x, long int y);
				void set_map_flag(const std::string& map, long int x, long int y, const std::string& flag);
				std::string get_map_flag(const std::string& map, long int x, long int y);
				void save();
			private:
				
				void batchvalues(sql::Connection* conn, const std::vector<Value>& values);
				void batchflags(sql::Connection* conn, const std::vector<Flag>& flags);
				//void batchtiles(sql::Connection* conn, const std::vector<std::pair<Vec2<int>, Tile> >& tiles);
				//void setPixel(std::vector<unsigned char>& bytes, long int x, long int y, const Color& c);
			
				Engine* _engine;
				//std::vector<std::vector<Tile> > _tiles;
				std::map<std::string, std::vector<std::vector<float> > > _mapvals;
				std::map<std::string, std::vector<std::vector<std::string> > > _mapflags;
				long int _xsize;
				long int _ysize;
		};
	}
}