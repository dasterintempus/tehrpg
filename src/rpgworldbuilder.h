#pragma once

#include <vector>
#include <list>
#include <utility>

#include "vec2.h"
#include "lodepng.h"

namespace teh
{
	namespace RPG
	{
		/*
		struct Color
		{
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		};
		
		struct MapTunneler
		{
			Vec2<int> dir;
			int width;
			Vec2<int> pos;
		};
		
		struct MassInfo
		{
			float density;
			Vec2<int> pos;
		};
		
		class MapBuilder
		{
			public:
				MapBuilder(int xblocks, int yblocks, int blocksize);
				void build();
				void buildblock(int xblock, int yblock);
				//void buildtunnelerblock(int xblock, int yblock);
				void buildcells();
				void connectblocks(int xblocka, int yblocka, int xblockb, int yblockb);
				void write(const std::string& filename);
			private:
				void setNextPos(int xblock, int yblock);
				void setPixel(std::vector<unsigned char>& bytes, long int x, long int y, const Color& c);
				void init_tiles();
				void clamp_tunneler(MapTunneler& mt, Quad<int> bounds);
				void wrap_tunneler(MapTunneler& mt);
				static Quad<int> build_quad(const MapTunneler& current, const MapTunneler& last);
				void clear_quad_tiles(const Quad<int>& quad, int step);
				float density(const Vec2<int>& center, int radius);
				float density(const Quad<int>& rect);
				MassInfo big_mass();
				MassInfo get_mass();
				MassInfo big_mass(const Quad<int>& rect);
				MassInfo get_mass(const Quad<int>& rect);
			
				static bool compare_massinfo(const MassInfo& a, const MassInfo& b);
			
				std::vector<std::vector<int> > _tiles;
				int _xsize;
				int _xblocks;
				int _ysize;
				int _yblocks;
				int _blocksize;
				int _numsteps;
				int _curpoint;
				int _densitycheckradius;
				Vec2<int> _nextpos;
		};
		*/
	}
}
