#include "rpgworldbuilder1.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <functional>

namespace teh
{	
	void RPGWorldBuilder1_solidifier(std::vector<std::vector<Tile> >& tiles, int x, int y)
	{
		std::cout << "drawer called" << std::endl;
		tiles[x][y].solid = true;
	}
	
	std::vector<std::vector<Tile> > RPGWorldBuilder1(unsigned long int xsize, unsigned long int ysize, unsigned short int numwalkers, unsigned short int walkeriterations)
	{
		//Debug purposes
		#ifdef TEHDEBUG
		srand(100);
		#endif
		
		//init tiles
		std::vector<std::vector<Tile> > tiles;
		for (unsigned long int x = 0; x < xsize;x++)
		{
			tiles.push_back(std::vector<Tile>());
			for (unsigned long int y = 0; y < ysize;y++)
			{
				Tile t;
				t.solid = false;
				tiles[x].push_back(t);
			}
		}
		
		//for determining scale
		float highestx = 0.0f;
		float highesty = 0.0f;
		
		Vec2<float> last_rpos(0.0f, 0.0f);
		Vec2<float> last_lpos(0.0f, 0.0f);
		
		for (unsigned int dwn = 0;dwn < numwalkers;dwn++)
		{
			std::vector<Vec2<float> > wallpositions_r;
			std::vector<Vec2<float> > wallpositions_l;
			
			DrunkenWalker dw;
			dw.angle = to_radians<float>(90);
			dw.hallwidth = (last_rpos - last_lpos).length();
			if (dw.hallwidth == 0.0f)
				dw.hallwidth = (rand()%1000)/100.0f + 3.0f;
			dw.pos = (last_rpos + last_lpos)/2.0f;
			
			for (unsigned int n = 0; n < walkeriterations; n++)
			{
				//Add positions to wallpositions
				Vec2<float> rpos = dw.pos + (Vec2<float>::from_angle(to_radians<float>(-90)+dw.angle).unit() * (dw.hallwidth/2.0f));
				Vec2<float> lpos = dw.pos + (Vec2<float>::from_angle(to_radians<float>(90)+dw.angle).unit() * (dw.hallwidth/2.0f));
				
				wallpositions_r.push_back(rpos);
				wallpositions_l.push_back(lpos);
				
				if (fabs(rpos.x()) > highestx) highestx = fabs(rpos.x());
				if (fabs(lpos.x()) > highestx) highestx = fabs(lpos.x());
				
				if (fabs(rpos.y()) > highesty) highesty = fabs(rpos.y());
				if (fabs(lpos.y()) > highesty) highesty = fabs(lpos.y());
				
				//Drunkenly walk
				float distance = (rand()%300)/100.0f+1.0f;
				Vec2<float> delta = Vec2<float>::from_angle(dw.angle).unit() * distance;
				dw.pos = dw.pos + delta;
				
				//Vary angle
				dw.angle += to_radians<float>(((rand()%2000)/100.0f) - 10.0f);
				if (dw.angle >= PI*2) dw.angle -= PI*2;
				else if (dw.angle < 0) dw.angle += PI*2;
				
				//Vary hallway size
				dw.hallwidth += (rand()%300)/100.0f - 1.85f;
				if (dw.hallwidth < 3.0f) dw.hallwidth = 3.0f;
				else if (dw.hallwidth > 15.0f) dw.hallwidth = 15.0f;
			}
			
			std::cout << highestx << " " << highesty << std::endl;
			
			std::vector<Vec2<float> > tilemapcoords;
			
			for (unsigned int n = 0;n < wallpositions_r.size();n++)
			{
				Vec2<float> rpos = wallpositions_r[n];
				
				if (n == (wallpositions_r.size()-1)/2)
				{
					last_rpos = rpos;
				}
				
				std::cout << "Wall position right: " << rpos.x() << ", " << rpos.y() << std::endl;
				
				float x = rpos.x();
				float y = rpos.y();
				
				std::cout << "x: " << x << " y: " << y << std::endl;
				
				x = x / highestx;
				y = y / highesty;
				
				std::cout << "x: " << x << " y: " << y << std::endl;
				
				x = x * ((xsize-1)/2);
				y = y * ((ysize-1)/2);

				std::cout << "x: " << x << " y: " << y << std::endl;
				
				x = x + ((xsize-1)/2);
				y = y + ((ysize-1)/2);
				
				std::cout << "x: " << x << " y: " << y << std::endl;
				
				std::cout << "Tile wall position right: " << x << ", " << y << std::endl;
				tilemapcoords.push_back(Vec2<float>(x, y));
			}
			for (unsigned int n = wallpositions_l.size()-1;n > 0;n--)
			{
				Vec2<float> lpos = wallpositions_l[n];
				
				if (n == (wallpositions_l.size()-1)/2)
				{
					last_lpos = lpos;
				}
				
				std::cout << "Wall position left: " << lpos.x() << ", " << lpos.y() << std::endl;
				
				float x = lpos.x();
				float y = lpos.y();
				
				std::cout << "x: " << x << " y: " << y << std::endl;
				
				x = x / highestx;
				y = y / highesty;
				
				std::cout << "x: " << x << " y: " << y << std::endl;
				
				x = x * ((xsize-1)/2);
				y = y * ((ysize-1)/2);

				std::cout << "x: " << x << " y: " << y << std::endl;
				
				x = x + ((xsize-1)/2);
				y = y + ((ysize-1)/2);
				
				std::cout << "x: " << x << " y: " << y << std::endl;
				
				std::cout << "Tile wall position left: " << x << ", " << y << std::endl;
				tilemapcoords.push_back(Vec2<float>(x, y));
				
			}
			
			for (unsigned int n = 0;n < tilemapcoords.size();n++)
			{
				Vec2<float> cur = tilemapcoords[n];
				Vec2<float> next = tilemapcoords[(n+1)%tilemapcoords.size()];
				
				if (n == tilemapcoords.size()/2)
					continue;
				
				Bresenham(Vec2<int>(cur.x(), cur.y()), Vec2<int>(next.x(), next.y()), std::bind(RPGWorldBuilder1_solidifier, std::ref(tiles), std::placeholders::_1, std::placeholders::_2));
			}
		}
		
		//Debug purposes
		#ifdef TEHDEBUG
		srand(time(0));
		#endif
		
		return tiles;
	}
	
	
}