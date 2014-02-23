#pragma once

#include <vector>

#include "rpgworld.h"
#include "vec2.h"

namespace teh
{
	struct MapTunneler
	{
		Vec2<float> dir;
		float width;
		Vec2<float> pos;
	};
	
	class MapTunnelerBuilder
	{
		public:
			MapTunnelerBuilder(unsigned long int numsteps);
			std::vector<std::vector<Tile> > build(unsigned long int xsize, unsigned long int ysize);
		
		private:
			void init_tiles();
			void clamp_tunneler(MapTunneler& mt);
			static Quad<float> build_quad(const MapTunneler& current, const MapTunneler& last);
			void clear_quad_tiles(const Quad<float>& quad);
		
			std::vector<std::vector<Tile> > _tiles;
			unsigned long int _xsize;
			unsigned long int _ysize;
			unsigned long int _numsteps;
	};
}