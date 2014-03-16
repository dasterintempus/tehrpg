#include "rpgsystem.h"
#include "rpgworld.h"
#include "exceptions.h"

namespace teh
{
	namespace RPG
	{
		namespace Lua
		{
			int WP_save_fcall(lua_State* L)
			{
				int argc = lua_gettop(L);
				if (argc != 0)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				void* ud = lua_touserdata(L, lua_upvalueindex(1));
				if (!ud)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				World* world = static_cast<World*>(ud);
				
				world->save();
				
				return 0;
			}
			
			int WP_load_fcall(lua_State* L)
			{
				int argc = lua_gettop(L);
				if (argc != 0)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				void* ud = lua_touserdata(L, lua_upvalueindex(1));
				if (!ud)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				World* world = static_cast<World*>(ud);
				
				world->load();
				
				return 0;
			}
			
			int WP_init_size_fcall(lua_State* L)
			{
				int argc = lua_gettop(L);
				if (argc != 2)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				void* ud = lua_touserdata(L, lua_upvalueindex(1));
				if (!ud)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				World* world = static_cast<World*>(ud);
				int xsize = (int)lua_tonumber(L, 1);
				int ysize = (int)lua_tonumber(L, 2);
				lua_pop(L, 2);
				
				world->init_size(xsize, ysize);
				
				return 0;
			}
			
			int WP_size_fcall(lua_State* L)
			{
				int argc = lua_gettop(L);
				if (argc != 0)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				void* ud = lua_touserdata(L, lua_upvalueindex(1));
				if (!ud)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				World* world = static_cast<World*>(ud);
				
				lua_pushnumber(L, world->xsize());
				lua_pushnumber(L, world->ysize());
				
				return 2;
			}
			
			int WP_set_map_value_fcall(lua_State* L)
			{
				int argc = lua_gettop(L);
				if (argc != 4)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				void* ud = lua_touserdata(L, lua_upvalueindex(1));
				if (!ud)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				World* world = static_cast<World*>(ud);
				std::string map = lua_tostring(L, 1);
				int x = (int)lua_tonumber(L, 2) - 1;
				int y = (int)lua_tonumber(L, 3) - 1;
				float value = (float)lua_tonumber(L, 4);
				lua_pop(L, 4);
				
				world->set_map_value(map, x, y, value);
				
				return 0;
			}
			
			int WP_get_map_value_fcall(lua_State* L)
			{
				int argc = lua_gettop(L);
				if (argc != 3)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				void* ud = lua_touserdata(L, lua_upvalueindex(1));
				if (!ud)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				World* world = static_cast<World*>(ud);
				std::string map = lua_tostring(L, 1);
				int x = (int)lua_tonumber(L, 2) - 1;
				int y = (int)lua_tonumber(L, 3) - 1;
				lua_pop(L, 3);
				
				float value = world->get_map_value(map, x, y);
				lua_pushnumber(L, value);
				
				return 1;
			}
			
			int WP_set_map_flag_fcall(lua_State* L)
			{
				int argc = lua_gettop(L);
				if (argc != 4)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				void* ud = lua_touserdata(L, lua_upvalueindex(1));
				if (!ud)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				World* world = static_cast<World*>(ud);
				std::string map = lua_tostring(L, 1);
				int x = (int)lua_tonumber(L, 2) - 1;
				int y = (int)lua_tonumber(L, 3) - 1;
				std::string flag = lua_tostring(L, 4);
				lua_pop(L, 4);
				
				world->set_map_flag(map, x, y, flag);
				
				return 0;
			}
			
			int WP_get_map_flag_fcall(lua_State* L)
			{
				int argc = lua_gettop(L);
				if (argc != 3)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				void* ud = lua_touserdata(L, lua_upvalueindex(1));
				if (!ud)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				World* world = static_cast<World*>(ud);
				std::string map = lua_tostring(L, 1);
				int x = (int)lua_tonumber(L, 2) - 1;
				int y = (int)lua_tonumber(L, 3) - 1;
				lua_pop(L, 3);
				
				std::string flag = world->get_map_flag(map, x, y);
				lua_pushstring(L, flag.c_str());
				
				return 1;
			}
			
			int WP_mm_index(lua_State* L)
			{
				int argc = lua_gettop(L);
				if (argc != 2)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				void* ud = lua_touserdata(L, 1);
				if (!ud)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				Proxy* wp = static_cast<Proxy*>(ud);
				if (wp->type != ProxyWorld)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				
				std::string key = lua_tostring(L, 2);
				lua_pop(L, 2);
				World* world = wp->world;
				
				if (key == "save")
				{
					lua_pushlightuserdata(L, world);
					lua_pushcclosure(L, &WP_save_fcall, 1);
					
					return 1;
				}
				else if (key == "load")
				{
					lua_pushlightuserdata(L, world);
					lua_pushcclosure(L, &WP_load_fcall, 1);
					
					return 1;
				}
				else if (key == "init_size")
				{
					lua_pushlightuserdata(L, world);
					lua_pushcclosure(L, &WP_init_size_fcall, 1);
					
					return 1;
				}
				else if (key == "size")
				{
					lua_pushlightuserdata(L, world);
					lua_pushcclosure(L, &WP_size_fcall, 1);
					
					return 1;
				}
				else if (key == "set_map_value")
				{
					lua_pushlightuserdata(L, world);
					lua_pushcclosure(L, &WP_set_map_value_fcall, 1);
					
					return 1;
				}
				else if (key == "get_map_value")
				{
					lua_pushlightuserdata(L, world);
					lua_pushcclosure(L, &WP_get_map_value_fcall, 1);
					
					return 1;
				}
				else if (key == "set_map_flag")
				{
					lua_pushlightuserdata(L, world);
					lua_pushcclosure(L, &WP_set_map_flag_fcall, 1);
					
					return 1;
				}
				else if (key == "get_map_flag")
				{
					lua_pushlightuserdata(L, world);
					lua_pushcclosure(L, &WP_get_map_flag_fcall, 1);
					
					return 1;
				}
				return 0;
			}
			
			void pushWorld(lua_State* L, World* world)
			{
				//make the WorldProxy
				Proxy wp;
				wp.world = world;
				wp.type = ProxyWorld;
				
				//make the userdata and copy the WorldProxy into it
				void* ud = lua_newuserdata(L, sizeof(wp));
				*static_cast<Proxy*>(ud) = wp;
				
				//make the metatable
				lua_newtable(L);
				lua_pushstring(L, "__index");
				lua_pushcfunction(L, &WP_mm_index);
				lua_settable(L, -3);
				lua_setmetatable(L, -2);
			}
		}
	}
}