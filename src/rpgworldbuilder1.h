#pragma once

#include <vector>

#include "rpgworld.h"
#include "vec2.h"

namespace teh
{
	struct DrunkenWalker
	{
		float angle;
		float hallwidth;
		Vec2<float> pos;
	};
	
	std::vector<std::vector<Tile> > RPGWorldBuilder1(unsigned long int xsize, unsigned long int ysize, unsigned short int numwalkers, unsigned short int walkeriterations);
}