#include "rpgworldbuilder.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <functional>
#include <algorithm>
#include <sstream>

/*
namespace teh
{
	namespace RPG
	{
		MapBuilder::MapBuilder(int xblocks, int yblocks, int blocksize)
			: _numsteps(0), _curpoint(0), _xblocks(xblocks), _yblocks(yblocks), _blocksize(blocksize), _xsize(xblocks*blocksize), _ysize(yblocks*blocksize), _densitycheckradius(32)
		{

		}
		
		void MapBuilder::build()
		{
			init_tiles();
			buildcells();
			
			for (int x = 0; x < _xblocks-1;x++)
			{
				for (int y = 0; y < _yblocks;y++)
				{
					connectblocks(x,y, x+1,y);
				}
			}
			for (int x = 0; x < _xblocks;x++)
			{
				for (int y = 0; y < _yblocks-1;y++)
				{
					connectblocks(x,y, x,y+1);
				}
			}
		}
		
		void MapBuilder::buildtunnelerblock(int xblock, int yblock)
		{
			_curpoint = 0;
			MapTunneler current;
			MapTunneler last;
			
			int roll = rand()%4;
			if (roll == 0)
			{
				last.pos = Vec2<int>(xblock*_blocksize, yblock*_blocksize);
				current.pos = Vec2<int>(xblock*_blocksize, yblock*_blocksize);
			}
			else if (roll == 1)
			{
				last.pos = Vec2<int>((xblock+1)*_blocksize, yblock*_blocksize);
				current.pos = Vec2<int>((xblock+1)*_blocksize, yblock*_blocksize);
			}
			else if (roll == 2)
			{
				last.pos = Vec2<int>((xblock+1)*_blocksize, (yblock+1)*_blocksize);
				current.pos = Vec2<int>((xblock+1)*_blocksize, (yblock+1)*_blocksize);
			}
			else if (roll == 3)
			{
				last.pos = Vec2<int>(xblock*_blocksize, (yblock+1)*_blocksize);
				current.pos = Vec2<int>(xblock*_blocksize, (yblock+1)*_blocksize);
			}
			last.dir = Vec2<int>(0, 0);
			current.dir = Vec2<int>(0, 0);
			last.width = 0;
			current.width = (rand()%3) + 2;
			
			setNextPos(xblock, yblock);
			
			Quad<int> block = Quad<int>::rectangle(xblock*_blocksize, yblock*_blocksize, (xblock+1)*_blocksize, (yblock+1)*_blocksize);
			
			int step = 0;
			
			while (_curpoint < _numpoints)
			{
				std::cout << "***step*** #" << step << std::endl;
				
				std::cout << "current.width " << current.width << std::endl;
				std::cout << "current.pos " << current.pos << std::endl;
				std::cout << "current.dir " << current.dir << std::endl;
				
				Quad<int> justmoved = MapBuilder::build_quad(current, last);
				std::cout << "justmoved: " << justmoved << std::endl;
				int clearroll = rand()%100;
				clearroll -= ((5 * _curpoint) / _numpoints);
				if (clearroll < _clearchance)
					clear_quad_tiles(justmoved, step);
				
				//done with loop
				last = current;
				float currentdensity = density(current.pos, _densitycheckradius);
				std::cout << "density " << currentdensity << std::endl;
				//check for too close to nextpos
				if ((_nextpos - current.pos).length() <= 10 || justmoved.fuzzycontains(_nextpos))
				{
					setNextPos(xblock, yblock);
					_curpoint++;
				}
				//redo current.dir here
				std::cout << "nextpos " << _nextpos << std::endl;
				float anglebetween = (_nextpos - current.pos).angle();
				std::cout << "anglebetween " << to_degrees<float>(anglebetween) << std::endl;
				float realangle = anglebetween + (to_radians<float>((rand()%3)-6)/currentdensity);
				int distance = (int)((rand()%3)+3) / currentdensity;
				distance *= sqrt((_nextpos - current.pos).length());
				if (distance > (_xsize + _ysize) / 32) distance = (_xsize + _ysize)/32;
				Vec2<float> ndir = Vec2<float>::from_angle(realangle) * distance;
				current.dir = Vec2<int>(ndir.x(), ndir.y());
				std::cout << "new current.dir " << current.dir << std::endl;
				//redo current.width here
				current.width = (int)floor(((rand()%4) + 4) / (currentdensity));
				int scaledwidth = (int)(current.width * (_curpoint/(float)(_numpoints*3.0f/2.0f)));
				current.width = (current.width + current.width + scaledwidth)/3;
				std::cout << "width " << current.width << std::endl;
				current.pos = current.pos + current.dir;
				clamp_tunneler(current, block);
				
				//std::stringstream fn;
				//fn << "world" << step << ".png";
				//write(fn.str());
				step++;
			}
			
			_numsteps += step;
			//DONE
		}
		
		void MapBuilder::buildcells()
		{
			for (int x = 0; x < _xsize ; x++)
			{
				for (int y = 0; y < _ysize; y++)
				{
					_tiles[x][y] = rand()%2; //Fill in with 50% randomness
				}
			}
			
			for (int step = 0;step < 8; step++)
			{
				std::list<std::pair<Vec2<int>, int> > changes;
				for (int x = 0; x < _xsize; x++)
				{
					for (int y = 0; y < _ysize; y++)
					{
						Quad<int> region = Quad<int>::rectangle(x-1, y-1, x+1, y+1);
						float regiondensity = density(region);
						if (regiondensity > 0.5f)
						{
							changes.push_back(std::make_pair(Vec2<int>(x,y), 1));
						}
						else
						{
							changes.push_back(std::make_pair(Vec2<int>(x,y), 0));
						}
					}
				}
				
				for (auto i = changes.begin(); i != changes.end(); i++)
				{
					Vec2<int> point = (*i).first;
					int nval = (*i).second;
					_tiles[point.x()][point.y()] = nval;
				}
				
				_numsteps++;
			}
		}
		
		void MapBuilder::connectblocks(int xblocka, int yblocka, int xblockb, int yblockb)
		{
			MapTunneler current;
			MapTunneler last;
			
			//Get start pos from blocka
			std::vector<Vec2<int> > opensquares;
			for (int x = xblocka*_blocksize; x < (xblocka+1)*_blocksize; x++)
			{
				for (int y = yblocka*_blocksize; y < (yblocka+1)*_blocksize; y++)
				{
					if (_tiles[x][y] == 0)
						opensquares.push_back(Vec2<int>(x,y));
				}
			}
			
			Vec2<int> startpos = opensquares[rand()%opensquares.size()];
			
			last.dir = Vec2<int>(0,0);
			current.dir = Vec2<int>(0,0);
			last.pos = startpos;
			current.pos = startpos;
			last.width = 0;
			current.width = (rand()%3) + 2;
			
			//Get dest pos from blockb
			opensquares.clear();
			for (int x = xblockb*_blocksize; x < (xblockb+1)*_blocksize; x++)
			{
				for (int y = yblockb*_blocksize; y < (yblockb+1)*_blocksize; y++)
				{
					if (_tiles[x][y] == 0)
						opensquares.push_back(Vec2<int>(x,y));
				}
			}
			
			Vec2<int> destpos = opensquares[rand()%opensquares.size()];
			
			int step = 0;
			
			Quad<int> bounds = Quad<int>::rectangle(xblocka*_blocksize, yblocka*_blocksize, (xblockb+1)*_blocksize, (yblockb+1)*_blocksize);
			
			while (true)
			{
				std::cout << "***step*** #" << step << std::endl;
				
				std::cout << "current.width " << current.width << std::endl;
				std::cout << "current.pos " << current.pos << std::endl;
				std::cout << "current.dir " << current.dir << std::endl;
				std::cout << "destpos " << destpos << std::endl;
				std::cout << "blocka " << xblocka << " " << yblocka << std::endl;
				std::cout << "blockb " << xblockb << " " << yblockb << std::endl;
				std::cout << "blocksize " << _blocksize << std::endl;
				std::cout << "bounds " << bounds << std::endl;
				
				Quad<int> justmoved = MapBuilder::build_quad(current, last);
				std::cout << "justmoved: " << justmoved << std::endl;
				int clearroll = rand()%100;
				//clearroll -= ((5 * _curpoint) / _numpoints);
				if (clearroll < 50)
					clear_quad_tiles(justmoved, step);
				
				//done with loop
				last = current;
				float currentdensity = density(current.pos, _densitycheckradius);
				std::cout << "density " << currentdensity << std::endl;
				//check for too close to nextpos
				if ((destpos - current.pos).length() <= 10 || justmoved.fuzzycontains(destpos))
				{
					break;
				}
				//redo current.dir here
				float anglebetween = (destpos - current.pos).angle();
				std::cout << "anglebetween " << to_degrees<float>(anglebetween) << std::endl;
				float realangle = anglebetween + (to_radians<float>((rand()%3)-6)/currentdensity);
				int distance = (int)((rand()%3)+3) / currentdensity;
				distance *= sqrt((destpos - current.pos).length());
				if (distance > (_xsize + _ysize) / 32) distance = (_xsize + _ysize)/32;
				Vec2<float> ndir = Vec2<float>::from_angle(realangle) * distance;
				current.dir = Vec2<int>(ndir.x(), ndir.y());
				std::cout << "new current.dir " << current.dir << std::endl;
				//redo current.width here
				current.width = (int)floor(((rand()%3) + 3) / (1.5f*currentdensity));
				//int scaledwidth = (int)(current.width * (_curpoint/(float)(_numpoints*3.0f/2.0f)));
				//current.width = (current.width + current.width + scaledwidth)/3;
				std::cout << "width " << current.width << std::endl;
				current.pos = current.pos + current.dir;
				clamp_tunneler(current, bounds);
				
				//std::stringstream fn;
				//fn << "world" << step << ".png";
				//write(fn.str());
				step++;
				if (step > 20)
					break;
			}
			
			_numsteps += step;
		}
		
		void MapBuilder::setNextPos(int xblock, int yblock)
		{
			Quad<int> block = Quad<int>::rectangle(xblock*_blocksize, yblock*_blocksize, (xblock+1)*_blocksize, (yblock+1)*_blocksize);
			MassInfo mi;
			if (rand()%2)
				mi = big_mass(block);
			else
				mi = get_mass(block);
			Vec2<int> delta = Vec2<int>(rand()%(_xsize/4), rand()%(_ysize/4));
			delta.rotate(to_radians<int>(rand()%360));
			_nextpos = mi.pos + delta;
			if (_nextpos.x() < xblock*_blocksize) _nextpos.x(xblock*_blocksize);
			if (_nextpos.y() < yblock*_blocksize) _nextpos.y(yblock*_blocksize);
			if (_nextpos.x() > (xblock+1)*_blocksize) _nextpos.x((xblock+1)*_blocksize);
			if (_nextpos.y() > (yblock+1)*_blocksize) _nextpos.y((yblock+1)*_blocksize);
		}
		
		void MapBuilder::write(const std::string& filename)
		{
			//Init byte array
			std::vector<unsigned char> bytes;
			for (long int x = 0; x < _xsize;x++)
			{
				for (long int y = 0; y < _ysize;y++)
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
					int t = _tiles[x][y];
					if (t)
					{
						Color c;
						c.r = (t*75)/_numsteps;
						c.g = (t*75)/_numsteps;
						c.b = (t*255)/_numsteps;
						c.a = 255;
						setPixel(bytes, x, y, c);
					}
					else
					{
						setPixel(bytes, x, y, white);
					}
				}
			}
			
			lodepng::encode(filename.c_str(), bytes, _xsize, _ysize);
		}
		
		void MapBuilder::setPixel(std::vector<unsigned char>& bytes, long int x, long int y, const Color& c)
		{
			int n = (4 * _xsize * y) + (4 * x);
			bytes[n] = c.r;
			bytes[n+1] = c.g;
			bytes[n+2] = c.b;
			bytes[n+3] = c.a;
		}
		
		void MapBuilder::init_tiles()
		{
			for (unsigned long int x = 0; x < _xsize;x++)
			{
				_tiles.push_back(std::vector<int>());
				for (unsigned long int y = 0; y < _ysize;y++)
				{
					_tiles[x].push_back(0);
				}
			}
		}
		
		void MapBuilder::clamp_tunneler(MapTunneler& mt, Quad<int> bounds)
		{
			if (mt.pos.x() < bounds.a().x()) mt.pos.x(bounds.a().x());
			if (mt.pos.y() < bounds.a().y()) mt.pos.y(bounds.a().y());
			if (mt.pos.x() > bounds.b().x()) mt.pos.x(bounds.b().x());
			if (mt.pos.y() > bounds.c().y()) mt.pos.y(bounds.c().y());
			if (mt.dir.x() == 0)
			{
				if (rand()%2)
					mt.dir.x(1);
				else
					mt.dir.x(-1);
			}
			if (mt.dir.y() == 0)
			{
				if (rand()%2)
					mt.dir.y(1);
				else
					mt.dir.y(-1);
			}
			if (mt.dir.length() > ((int)_xsize+(int)_ysize)/8)
			{
				mt.dir = mt.dir / 4;
			}
			if (mt.width > ((int)_xsize+(int)_ysize)/32)
			{
				mt.width = ((int)_xsize+(int)_ysize)/32;
			}
			else if (mt.width < -((int)_xsize+(int)_ysize)/32)
			{
				mt.width = -((int)_xsize+(int)_ysize)/32;
			}
		}
		
		void MapBuilder::wrap_tunneler(MapTunneler& mt)
		{
			while (mt.pos.x() < 0) mt.pos.x(mt.pos.x() + _xsize);
			while (mt.pos.y() < 0) mt.pos.y(mt.pos.y() + _ysize);
			while (mt.pos.x() > _xsize) mt.pos.x(mt.pos.x() - _xsize);
			while (mt.pos.y() > _ysize) mt.pos.y(mt.pos.y() - _ysize);
			if (mt.dir.x() == 0)
			{
				if (rand()%2)
					mt.dir.x(1);
				else
					mt.dir.x(-1);
			}
			if (mt.dir.y() == 0)
			{
				if (rand()%2)
					mt.dir.y(1);
				else
					mt.dir.y(-1);
			}
			if (mt.dir.length() > ((int)_xsize+(int)_ysize)/8)
			{
				mt.dir = mt.dir / 4;
			}
			if (mt.width > ((int)_xsize+(int)_ysize)/32)
			{
				mt.width = ((int)_xsize+(int)_ysize)/32;
			}
			else if (mt.width < -((int)_xsize+(int)_ysize)/32)
			{
				mt.width = -((int)_xsize+(int)_ysize)/32;
			}
		}
		
		Quad<int> MapBuilder::build_quad(const MapTunneler& current, const MapTunneler& last)
		{
			std::cout << "build_quad" << std::endl;
			
			//Build quad that we just moved
			Vec2<int> currentrightpos = Vec2<int>(current.width, 0).rotated(current.dir.angle()) + current.pos;
			Vec2<int> currentleftpos = Vec2<int>(-current.width, 0).rotated(current.dir.angle()) + current.pos;
			
			std::cout << "currentrightpos: " << currentrightpos << std::endl;
			std::cout << "currentleftpos: " << currentleftpos << std::endl;
			
			Vec2<int> lastrightpos = Vec2<int>(last.width, 0.0f).rotated(last.dir.angle()) + last.pos;
			Vec2<int> lastleftpos = Vec2<int>(-last.width, 0.0f).rotated(last.dir.angle()) + last.pos;
			
			std::cout << "lastrightpos: " << lastrightpos << std::endl;
			std::cout << "lastleftpos: " << lastleftpos << std::endl;
			
			Quad<int> q;
			q.a(currentrightpos);
			q.b(lastrightpos);
			q.c(lastleftpos);
			q.d(currentleftpos);
		
			return q;
		}
		
		void MapBuilder::clear_quad_tiles(const Quad<int>& quad, int step)
		{
			Quad<int> bounds = quad.bounding_rectangle<int>();
			std::cout << "Bounds: " << bounds << std::endl;
			for (int x = bounds.a().x(); x < bounds.b().x(); x++)
			{
				for (int y = bounds.a().y(); y < bounds.c().y(); y++)
				{
					if (x < 0 || y < 0 || x >= _xsize || y >= _ysize)
						continue;
					Vec2<int> testpoint(x,y);
					//std::cout << "test point: " << testpoint << std::endl;
					if (quad.fuzzycontains(testpoint))
					{
						//std::cout << "contains == true" << std::endl;
						_tiles[x][y] = step;
					}
				}
			}
		}
		
		float MapBuilder::density(const Vec2<int>& center, int radius)
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
							if (_tiles[realpos.x()][realpos.y()]==0)
								solidtiles++;
						}
					}
				}
			}
			if (totaltiles == 0) return 1;
			return ((float)(solidtiles))/((float)(totaltiles));
		}
		
		float MapBuilder::density(const Quad<int>& rect)
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
							if (_tiles[x][y]==0)
								solidtiles++;
						}
					}
				}
			}
			if (totaltiles == 0) return 1;
			return ((float)(solidtiles))/((float)(totaltiles));
		}
		
		MassInfo MapBuilder::big_mass()
		{
			return big_mass(Quad<int>::rectangle(0,0,_xsize,_ysize));
		}
		
		MassInfo MapBuilder::get_mass()
		{
			return get_mass(Quad<int>::rectangle(0,0,_xsize,_ysize));
		}
		
		MassInfo MapBuilder::big_mass(const Quad<int>& rect)
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
			std::sort(vec.begin(), vec.end(), &MapBuilder::compare_massinfo);
			
			return vec.back();
		}
		
		MassInfo MapBuilder::get_mass(const Quad<int>& rect)
		{
			int xsize = rect.a2b_distance()/2;
			int ysize = rect.b2c_distance()/2;
			
			if (xsize <= _xsize/10 || ysize <= _ysize/10)
			{
				MassInfo out;
				out.density = density(rect);
				out.pos = Vec2<int>(rect.a().x() + xsize, rect.a().y() + ysize);
			
				return out;
			}
			
			Quad<int> tl = Quad<int>::rectangle(rect.a().x(), rect.a().y(), rect.a().x() + xsize, rect.a().y() + ysize);
			Quad<int> tr = Quad<int>::rectangle(rect.a().x() + xsize, rect.a().y(), rect.a().x() + (xsize*2), rect.a().y() + ysize);
			Quad<int> br = Quad<int>::rectangle(rect.a().x() + xsize, rect.a().y() + ysize, rect.a().x() + (xsize*2), rect.a().y() + (ysize*2));
			Quad<int> bl = Quad<int>::rectangle(rect.a().x(), rect.a().y() + ysize, rect.a().x() + xsize, rect.a().y() + (ysize*2));
			
			MassInfo tlmass = get_mass(tl);
			MassInfo trmass = get_mass(tr);
			MassInfo brmass = get_mass(br);
			MassInfo blmass = get_mass(bl);

			MassInfo out;
			out.density = (tlmass.density + trmass.density + brmass.density + blmass.density)/4.0f;
			
			std::cout << "out density " << out.density << std::endl;
			
			Vec2<float> tlshifted = Vec2<float>(tlmass.pos.x(), tlmass.pos.y()) - Vec2<float>(rect.a().x() + xsize, rect.a().y() + ysize);
			Vec2<float> trshifted = Vec2<float>(trmass.pos.x(), trmass.pos.y()) - Vec2<float>(rect.a().x() + xsize, rect.a().y() + ysize);
			Vec2<float> blshifted = Vec2<float>(blmass.pos.x(), blmass.pos.y()) - Vec2<float>(rect.a().x() + xsize, rect.a().y() + ysize);
			Vec2<float> brshifted = Vec2<float>(brmass.pos.x(), brmass.pos.y()) - Vec2<float>(rect.a().x() + xsize, rect.a().y() + ysize);
			
			std::cout << "shifted vecs" << std::endl;
			std::cout << tlshifted << std::endl;
			std::cout << trshifted << std::endl;
			std::cout << blshifted << std::endl;
			std::cout << brshifted << std::endl;
			
			Vec2<float> tlscaled = tlshifted * tlmass.density;
			Vec2<float> trscaled = trshifted * trmass.density;
			Vec2<float> blscaled = blshifted * blmass.density;
			Vec2<float> brscaled = brshifted * brmass.density;
			
			std::cout << "scaled vecs" << std::endl;
			std::cout << tlscaled << std::endl;
			std::cout << trscaled << std::endl;
			std::cout << blscaled << std::endl;
			std::cout << brscaled << std::endl;
			
			Vec2<float> tempout = tlscaled + trscaled + blscaled + brscaled;
			std::cout << "unscaled tempout " << tempout << std::endl;
			tempout = tempout / out.density;
			std::cout << "scaled tempout "<< tempout << std::endl;
			
			out.pos = Vec2<int>((int)floor(tempout.x()), (int)floor(tempout.y()));
			out.pos = out.pos + Vec2<int>(rect.a().x() + xsize, rect.a().y() + ysize);
			
			return out;
		}
		
		bool MapBuilder::compare_massinfo(const MassInfo& a, const MassInfo& b)
		{
			return a.density < b.density;
		}
	}
}
*/