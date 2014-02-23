#pragma once	

#include <string>
#include <map>

#include "lodepng.h"
#include "typedefs.h"

#include <functional>

namespace teh
{
	struct Color
	{
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};
	
	struct Tile
	{
		bool solid;
	};
	
	class RPGWorld
	{
		public:
			RPGWorld(unsigned long int xsize, unsigned long int ysize, unsigned short int tilesize);
			void build(std::function<std::vector<std::vector<Tile> > (unsigned long int, unsigned long int)> builder);
			void savePNG(const std::string& filename);
		
		private:
			void setPixel(std::vector<unsigned char>& bytes, long int x, long int y, const Color& c);
			unsigned long int pixelwidth();
			unsigned long int pixelheight();
			unsigned long int _xsize;
			unsigned long int _ysize;
			unsigned short int _tilesize;
			std::vector<std::vector<Tile> > _tiles;
	};
}