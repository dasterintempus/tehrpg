#include "rpgworldbuilder2.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <functional>
#include <algorithm>

/*
namespace teh
{
	MapTunnelerBuilder::MapTunnelerBuilder(unsigned long int numsteps, unsigned int densitycheckradius)
		: _numsteps(numsteps), _densitycheckradius(densitycheckradius)
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
			float currentdensity = density(Vec2<int>((int)(floor(current.pos.x())), (int)(floor(current.pos.y()))), (int)_densitycheckradius);
			std::cout << "density " << currentdensity << std::endl;
			//redo current.dir here
			MassInfo mi = big_mass();
			Vec2<float> masspos(mi.pos.x(), mi.pos.y());
			std::cout << "masspos " << masspos << std::endl;
			float anglebetween = (masspos - current.pos).angle();
			std::cout << "anglebetween " << to_degrees<float>(anglebetween) << std::endl;
			float realangle = anglebetween + to_radians<float>((rand()%30)-60);
			float distance = (((rand()%100)/100.0f)+1.0f);
			current.dir = Vec2<float>::from_angle(realangle) * distance;
			std::cout << "new current.dir " << current.dir << std::endl;
			//redo current.width here
			current.width = (((rand()%400)/100.0f) + 3.5f) / (currentdensity * currentdensity);
			std::cout << "width " << current.width << std::endl;
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
				if (x < 0 || y < 0 || x >= _xsize || y >= _ysize)
					continue;
				Vec2<float> testpoint(x,y);
				//std::cout << "test point: " << testpoint << std::endl;
				if (quad.contains(testpoint))
				{
					//std::cout << "contains == true" << std::endl;
					_tiles[x][y].solid = false;
				}
			}
		}
	}
	
	float MapTunnelerBuilder::density(const Vec2<int>& center, int radius)
	{
		unsigned int totaltiles = 0;
		unsigned int solidtiles = 0;
		for (int x = -radius; x < radius; x++)
		{
			for (int y = -radius; y < radius; y++)
			{
				//I call this the "I'm-not-a-CS-major circle checking algorithm"
				int dist = Vec2<int>(x,y).length();
				//std::cout << "density check dist " << dist << std::endl;
				if (dist > radius)
					continue;
				
				Vec2<int> realpos = center + Vec2<int>(x, y);
				//std::cout << "density check realpos " << realpos << std::endl;
				
				if (realpos.x() >= 0 && realpos.x() < _xsize)
				{
					if (realpos.y() >= 0 && realpos.y() < _ysize)
					{
						totaltiles++;
						if (_tiles[realpos.x()][realpos.y()].solid)
							solidtiles++;
					}
				}
			}
		}
		if (totaltiles == 0) return 1;
		return ((float)(solidtiles))/((float)(totaltiles));
	}
	
	float MapTunnelerBuilder::density(const Quad<int>& rect)
	{
		unsigned int totaltiles = 0;
		unsigned int solidtiles = 0;
		
		Quad<int> bounds = rect.bounding_rectangle<int>(); //should be basically a copy, but just to be safe
		
		for (int x = bounds.a().x(); x < bounds.b().x(); x++)
		{
			for (int y = bounds.a().y(); y < bounds.c().y(); y++)
			{
				if (x >= 0 && x < _xsize)
				{
					if (y >= 0 && y < _ysize)
					{
						totaltiles++;
						if (_tiles[x][y].solid)
							solidtiles++;
					}
				}
			}
		}
		if (totaltiles == 0) return 1;
		return ((float)(solidtiles))/((float)(totaltiles));
	}
	
	MassInfo MapTunnelerBuilder::big_mass()
	{
		return big_mass(Quad<int>::rectangle(0,0,_xsize,_ysize));
	}
	
	MassInfo MapTunnelerBuilder::big_mass(const Quad<int>& rect)
	{
		int xsize = rect.a2b_distance()/2;
		int ysize = rect.b2c_distance()/2;
		
		MassInfo out;
		out.density = density(rect);
		out.pos = Vec2<int>(rect.a().x() + xsize, rect.a().y() + ysize);
		
		if (xsize <= _xsize/10 || ysize <= _ysize/10)
			return out;
		
		Quad<int> tl = Quad<int>::rectangle(rect.a().x(), rect.a().y(), rect.a().x() + xsize, rect.a().y() + ysize);
		Quad<int> tr = Quad<int>::rectangle(rect.a().x() + xsize, rect.a().y(), rect.a().x() + (xsize*2), rect.a().y() + ysize);
		Quad<int> br = Quad<int>::rectangle(rect.a().x() + xsize, rect.a().y() + ysize, rect.a().x() + (xsize*2), rect.a().y() + (ysize*2));
		Quad<int> bl = Quad<int>::rectangle(rect.a().x(), rect.a().y() + ysize, rect.a().x() + xsize, rect.a().y() + (ysize*2));
		
		MassInfo tlmass = big_mass(tl);
		MassInfo trmass = big_mass(tr);
		MassInfo brmass = big_mass(br);
		MassInfo blmass = big_mass(bl);

		std::vector<MassInfo> vec;
		vec.push_back(tlmass);
		vec.push_back(trmass);
		vec.push_back(brmass);
		vec.push_back(blmass);
		std::sort(vec.begin(), vec.end(), &MapTunnelerBuilder::compare_massinfo);
		
		return vec.back();
	}
	
	bool MapTunnelerBuilder::compare_massinfo(const MassInfo& a, const MassInfo& b)
	{
		return a.density < b.density;
	}
}
*/