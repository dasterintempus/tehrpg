#include "rpgworld.h"
#include <iostream>

namespace teh
{
	
	RPGWorld::RPGWorld(unsigned long int xsize, unsigned long int ysize, unsigned short int tilesize)
		: _xsize(xsize), _ysize(ysize), _tilesize(tilesize)
	{
		/*
		for (long int x = 0; x < _xsize;x++)
		{
			_tiles.push_back(std::vector<Tile>());
			for (long int y = 0; y < _ysize;y++)
			{
				Tile t;
				t.solid = false;
				_tiles[x].push_back(t);
			}
		}
		*/
		
		//_tiles[1][1].northwall = true;
		//_tiles[1][1].westwall = true;
		//_tiles[5][5].northwall = true;
		//_tiles[5][5].westwall = true;
		//_tiles[5][6].northwall = true;
		//_tiles[6][5].westwall = true;
	}
	
	void RPGWorld::build(std::function<std::vector<std::vector<Tile> > (unsigned long int, unsigned long int)> builder)
	{
		_tiles = builder(_xsize, _ysize);
	}
	
	void RPGWorld::savePNG(const std::string& filename)
	{
		//Init byte array
		std::vector<unsigned char> bytes;
		for (long int x = 0; x < pixelwidth();x++)
		{
			for (long int y = 0; y < pixelheight();y++)
			{
				bytes.push_back(0);
				bytes.push_back(0);
				bytes.push_back(0);
				bytes.push_back(0);
			}
		}
		
		Color white;
		white.r=255;
		white.g=255;
		white.b=255;
		white.a=255;
		
		Color black;
		black.r=50;
		black.g=50;
		black.b=50;
		black.a=255;
		
		for (long int x = 0; x < _xsize;x++)
		{
			for (long int y = 0; y < _ysize;y++)
			{
				Tile t = _tiles[x][y];
				for (unsigned short int px = 0; px < _tilesize; px++)
				{
					for (unsigned short int py = 0; py < _tilesize; py++)
					{
						long int pixelx = px + (x * _tilesize);
						long int pixely = py + (y * _tilesize);
						if (t.solid)
						{
							setPixel(bytes, pixelx, pixely, white);
						}
						else
						{
							//std::cout << "Empty tile" << pixelx << ", " << pixely << std::endl;
							setPixel(bytes, pixelx, pixely, black);
						}
					}
				}
			}
		}
		
		lodepng::encode(filename.c_str(), bytes, pixelwidth(), pixelheight());
	}
	
	void RPGWorld::setPixel(std::vector<unsigned char>& bytes, long int x, long int y, const Color& c)
	{
		int n = (4 * pixelwidth() * y) + (4 * x);
		bytes[n] = c.r;
		bytes[n+1] = c.g;
		bytes[n+2] = c.b;
		bytes[n+3] = c.a;
	}
	
	
	unsigned long int RPGWorld::pixelwidth()
	{
		return _tilesize * _xsize;
	}
	
	unsigned long int RPGWorld::pixelheight()
	{
		return _tilesize * _ysize;
	}
}