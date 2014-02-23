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
	
	struct MassInfo
	{
		float density;
		Vec2<int> pos;
	};
	
	class MapTunnelerBuilder
	{
		public:
			MapTunnelerBuilder(unsigned long int numsteps, unsigned int densitycheckradius = 15);
			std::vector<std::vector<Tile> > build(unsigned long int xsize, unsigned long int ysize);
		
		private:
			void init_tiles();
			void clamp_tunneler(MapTunneler& mt);
			static Quad<float> build_quad(const MapTunneler& current, const MapTunneler& last);
			void clear_quad_tiles(const Quad<float>& quad);
			float density(const Vec2<int>& center, int radius);
			float density(const Quad<int>& rect);
			MassInfo big_mass();
			MassInfo big_mass(const Quad<int>& rect);
		
			static bool compare_massinfo(const MassInfo& a, const MassInfo& b);
		
			std::vector<std::vector<Tile> > _tiles;
			unsigned long int _xsize;
			unsigned long int _ysize;
			unsigned long int _numsteps;
			unsigned int _densitycheckradius;
	};
}