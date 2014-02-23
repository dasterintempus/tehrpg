#include "rpgworldbuilder2.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <functional>

namespace teh
{
	MapTunnelerBuilder::MapTunnelerBuilder(unsigned long int numsteps)
		: _numsteps(numsteps)
	{
		
	}
	
	std::vector<std::vector<Tile> > MapTunnelerBuilder::build(unsigned long int xsize, unsigned long int ysize)
	{
		#ifdef TEHDEBUG
		srand(100);
		#endif
		
		_xsize = xsize;
		_ysize = ysize;
		
		init_tiles();
		
		MapTunneler current;
		MapTunneler last;
		
		last.dir = Vec2<float>(0.0f, 0.0f);
		last.pos = Vec2<float>(0.0f, 0.0f);
		last.width = 0.0f;

		current.dir = Vec2<float>::from_angle(to_radians<float>((rand()%30)+30)) * (((rand()%100)/100.0f)+1.0f);
		current.pos = Vec2<float>(0.0f, 0.0f);
		current.width = ((rand()%300)/100.0f) + 3;
		
		for (unsigned long int step = 0; step < _numsteps; step++)
		{
			std::cout << "current.width " << current.width << std::endl;
			std::cout << "current.pos " << current.pos << std::endl;
			std::cout << "current.dir " << current.dir << std::endl;
			
			Quad<float> justmoved = MapTunnelerBuilder::build_quad(current, last);
			std::cout << "justmoved: " << justmoved << std::endl;
			clear_quad_tiles(justmoved);
			
			//done with loop
			last = current;
			//TODO: redo current.dir here
			current.pos = current.pos + current.dir;
			clamp_tunneler(current);
		}
		
		//DONE
		#ifdef TEHDEBUG
		srand(time(0));
		#endif
		return _tiles;
	}
	
	void MapTunnelerBuilder::init_tiles()
	{
		for (unsigned long int x = 0; x < _xsize;x++)
		{
			_tiles.push_back(std::vector<Tile>());
			for (unsigned long int y = 0; y < _ysize;y++)
			{
				Tile t;
				t.solid = true;
				_tiles[x].push_back(t);
			}
		}
	}
	
	void MapTunnelerBuilder::clamp_tunneler(MapTunneler& mt)
	{
		if (mt.pos.x() < 0.0f) mt.pos.x(0.0f);
		if (mt.pos.y() < 0.0f) mt.pos.y(0.0f);
		if (mt.pos.x() > _xsize) mt.pos.x(_xsize);
		if (mt.pos.y() > _ysize) mt.pos.y(_ysize);
	}
	
	Quad<float> MapTunnelerBuilder::build_quad(const MapTunneler& current, const MapTunneler& last)
	{
		std::cout << "build_quad" << std::endl;
		
		//Build quad that we just moved
		Vec2<float> currentrightpos = Vec2<float>(current.width, 0.0f).rotated(current.dir.angle()) + current.pos;
		Vec2<float> currentleftpos = Vec2<float>(-current.width, 0.0f).rotated(current.dir.angle()) + current.pos;
		
		std::cout << "currentrightpos: " << currentrightpos << std::endl;
		std::cout << "currentleftpos: " << currentleftpos << std::endl;
		
		Vec2<float> lastrightpos = Vec2<float>(last.width, 0.0f).rotated(last.dir.angle()) + last.pos;
		Vec2<float> lastleftpos = Vec2<float>(-last.width, 0.0f).rotated(last.dir.angle()) + last.pos;
		
		std::cout << "lastrightpos: " << lastrightpos << std::endl;
		std::cout << "lastleftpos: " << lastleftpos << std::endl;
		
		Quad<float> q;
		q.a(currentrightpos);
		q.b(lastrightpos);
		q.c(lastleftpos);
		q.d(currentleftpos);
	
		return q;
	}
	
	void MapTunnelerBuilder::clear_quad_tiles(const Quad<float>& quad)
	{
		Quad<int> bounds = quad.bounding_rectangle<int>();
		std::cout << "Bounds: " << bounds << std::endl;
		for (int x = bounds.a().x(); x < bounds.b().x(); x++)
		{
			for (int y = bounds.a().y(); y < bounds.c().y(); y++)
			{
				if (x < 0 || y < 0)
					continue;
				Vec2<float> testpoint(x,y);
				std::cout << "test point: " << testpoint << std::endl;
				if (quad.contains(testpoint))
				{
					std::cout << "contains == true" << std::endl;
					_tiles[x][y].solid = false;
				}
			}
		}
	}
}