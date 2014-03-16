#include "rpgsystem.h"
#include "rpgengine.h"
#include "rpgcomponent.h"
#include "rpgentity.h"
#include "rpgaction.h"
#include "rpgworld.h"
#include "simplexnoise.h"
#include "exceptions.h"

namespace teh
{
	namespace RPG
	{
		namespace Lua
		{
			int Util_noise2d(lua_State* L)
			{
				int argc = lua_gettop(L);
				if (argc != 2)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				float x = (float)lua_tonumber(L, 1);
				float y = (float)lua_tonumber(L, 2);
				lua_pop(L, 2);
				
				float result = raw_noise_2d(x, y);
				lua_pushnumber(L, result);
				return 1;
			}
			
			int Util_octave_noise2d(lua_State* L)
			{
				int argc = lua_gettop(L);
				if (argc != 5)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				float octaves = (float)lua_tonumber(L, 1);
				float persistence = (float)lua_tonumber(L, 2);
				float scale = (float)lua_tonumber(L, 3);
				float x = (float)lua_tonumber(L, 4);
				float y = (float)lua_tonumber(L, 5);
				lua_pop(L, 5);
				
				float result = octave_noise_2d(octaves, persistence, scale, x, y);
				lua_pushnumber(L, result);
				return 1;
			}
			
			int Util_noise3d(lua_State* L)
			{
				int argc = lua_gettop(L);
				if (argc != 3)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				float x = (float)lua_tonumber(L, 1);
				float y = (float)lua_tonumber(L, 2);
				float z = (float)lua_tonumber(L, 3);
				lua_pop(L, 3);
				
				float result = raw_noise_3d(x, y, z);
				lua_pushnumber(L, result);
				return 1;
			}
			
			int Util_octave_noise3d(lua_State* L)
			{
				int argc = lua_gettop(L);
				if (argc != 6)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				float octaves = (float)lua_tonumber(L, 1);
				float persistence = (float)lua_tonumber(L, 2);
				float scale = (float)lua_tonumber(L, 3);
				float x = (float)lua_tonumber(L, 4);
				float y = (float)lua_tonumber(L, 5);
				float z = (float)lua_tonumber(L, 6);
				lua_pop(L, 6);
				
				float result = octave_noise_3d(octaves, persistence, scale, x, y, z);
				lua_pushnumber(L, result);
				return 1;
			}
			
			int Util_image(lua_State* L)
			{
				int argc = lua_gettop(L);
				if (argc != 4)
				{
					lua_pop(L, argc);
					return 0;
				}
				std::string filename = lua_tostring(L, 1);
				unsigned int xsize = (unsigned int)lua_tonumber(L, 2);
				unsigned int ysize = (unsigned int)lua_tonumber(L, 3);
				std::vector<unsigned char> bytes;
				
				lua_pushnil(L); //push dummy first key
				while (lua_next(L, 4) != 0)
				{
					//Key = -2
					//Value = -1
					if (lua_type(L, -2) == LUA_TNUMBER && lua_type(L, -1) == LUA_TNUMBER)
					{
						unsigned int key = (unsigned int)lua_tonumber(L, -2);
						unsigned char value = (unsigned char)lua_tonumber(L, -1);
						
						while (key-1 >= bytes.size())
						{
							bytes.push_back(0);
						}
						bytes[key-1] = value;
					}
					//remove value, keep key
					lua_pop(L, 1);
				}
				
				lua_pop(L, 4);
				
				if (bytes.size() == xsize*ysize*4)
				{
					lodepng::encode(filename.c_str(), bytes, xsize, ysize);
					lua_pushboolean(L, true);
				}
				else
				{
					std::cerr << "Received byte array of size " << bytes.size() << " expecting " << xsize*ysize*4 << std::endl;
					lua_pushboolean(L, false);
				}
				
				return 1;
			}
			
			void pushStringVector(lua_State* L, const std::vector<std::string>& vec)
			{
				lua_newtable(L);
				for (unsigned int n = 0; n < vec.size(); n++)
				{
					lua_pushnumber(L, n+1);
					lua_pushstring(L, vec[n].c_str());
					lua_settable(L, -3);
				}
			}
			
			bool copyStackValue(lua_State* sourceL, int index, lua_State* destL)
			{
				void* received_ud = 0;
				Proxy* received = 0;
				bool pushed = false;
				switch (lua_type(sourceL, index))
				{
					case LUA_TNUMBER:
						lua_pushnumber(destL, lua_tonumber(sourceL, index));
						pushed = true;
						break;
					case LUA_TSTRING:
						lua_pushstring(destL, lua_tostring(sourceL, index));
						pushed = true;
						break;
					case LUA_TBOOLEAN:
						lua_pushboolean(destL, lua_toboolean(sourceL, index));
						pushed = true;
						break;
					case LUA_TNIL:
						lua_pushnil(destL);
						pushed = true;
						break;
					case LUA_TTABLE:
						lua_pushnil(sourceL); //push dummy first key
						lua_newtable(destL); //add table to destination
						if (index < 1) index--;
						while (lua_next(sourceL, index) != 0)
						{
							//Key = -2
							//Value = -1
							
							//Push key to dest
							if (copyStackValue(sourceL, -2, destL))
							{
								//Check that it really copied the key
								//Now push value to dest
								if (copyStackValue(sourceL, -1, destL))
								{
									//Check that it really copied the value
									//Set the key in dest
									lua_settable(destL, -3);
								}
								else
								{
									//Remove the 'key' from dest because we can't support this value type
									std::cerr << "Missing part of table in stack copy, due to unsupported value type for stack copy" << std::endl;
									lua_pop(destL, 1);
								}
							}
							else
							{
								std::cerr << "Missing part of table in stack copy, due to unsupported key type for stack copy" << std::endl;
							}
							lua_pop(sourceL, 1); //pop the value so iteration can continue
						}
						pushed = true;
						break;
					case LUA_TUSERDATA:
						std::cerr << "Copying userdata of type ";
						received_ud = lua_touserdata(sourceL, index);
						received = static_cast<Proxy*>(received_ud);
						if (received->type == ProxyComponent)
						{
							std::cerr << "Component" << std::endl;
							pushComponent(destL, received->component);
							pushed = true;
						}
						else if (received->type == ProxyEntity)
						{
							std::cerr << "Entity" << std::endl;
							pushEntity(destL, received->entity);
							pushed = true;
						}
						else if (received->type == ProxyEngine)
						{
							std::cerr << "Engine" << std::endl;
							pushEngine(destL, received->engine);
							pushed = true;
						}
						else if (received->type == ProxySystem)
						{
							std::cerr << "System" << std::endl;
							pushSystem(destL, received->system);
							pushed = true;
						}
						break;
					default:
						break;
				}
				//lua_pop(sourceL, 1);
				return pushed;
			}
			
			int copyStack(lua_State* sourceL, int startindex, lua_State* destL)
			{
				int argc = lua_gettop(sourceL);
				std::cerr << "copying stack of size " << argc << " starting at " << startindex << std::endl;
				int output = 0;

				for (unsigned int n = startindex; n <= argc; n++)
				{
					if (copyStackValue(sourceL, n, destL))
						output++;
					//lua_pop(sourceL, 1);
				}
				return output;
			}
			
			
		}
	}
}