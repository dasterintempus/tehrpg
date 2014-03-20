#include "rpgsystem.h"
#include "rpgengine.h"
#include "rpgentity.h"
#include "rpgcomponent.h"
#include "exceptions.h"
#include "mysql.h"
#include "stringutil.h"
#include <sstream>

namespace teh
{
	namespace RPG
	{
		namespace Lua
		{
			int NP_system_mm_index(lua_State* L)
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
				Proxy* np = static_cast<Proxy*>(ud);
				if (np->type != ProxyEngine)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				std::string key = lua_tostring(L, 2);
				lua_pop(L, 2);
				
				System* system = np->engine->get_system(key);
				if (system)
				{
					pushSystem(L, system);
					return 1;
				}
				return 0;
			}
			
			int NP_new_entity_fcall(lua_State* L)
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
				Engine* engine = static_cast<Engine*>(ud);
				
				std::string name = lua_tostring(L, 1);
				std::string type = lua_tostring(L, 2);
				
				pushEntity(L, new Entity(name, type, engine));
				
				return 1;
			}
			
			int NP_etc_fcall(lua_State* L)
			{
				int argc = lua_gettop(L);
				if (argc != 1 && argc != 2)
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
				Engine* engine = static_cast<Engine*>(ud);
				std::string key = lua_tostring(L, 1);
				if (argc == 1)
				{
					lua_pushstring(L, engine->get_etc(key).c_str());
					return 1;
				}
				if (argc == 2)
				{
					std::string value;
					if (lua_type(L, 2) == LUA_TNUMBER)
					{
						double dval = lua_tonumber(L, 2);
						std::stringstream conv;
						conv << dval;
						value = conv.str();
					}
					else
					{
						value = lua_tostring(L, 2);
					}
					
					engine->set_etc(key, value);
					return 0;
				}
				return 0;
			}
			
			int NP_select_fcall(lua_State* L)
			{
				int argc = lua_gettop(L);
				if (argc < 3 || argc%2 == 0) //Needs odd number of arguments
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
				Engine* engine = static_cast<Engine*>(ud);
				std::vector<std::string> tables;
				for (int argn = 4; argn <= argc; argn+=2)
				{
					std::string tablename = lua_tostring(L, argn);
					tablename += "Components";
					tables.push_back(tablename);
				}
				std::stringstream query;
				query << "SELECT `Entities`.`id` FROM `Entities` ";
				for (unsigned int n = 0;n < tables.size();n++)
				{
					query << "JOIN `" << tables[n] << "` ";
				}
				query << "WHERE ";
				if (lua_type(L, 1) != LUA_TNIL)
				{
					query << "`Entities`.`name` LIKE '" << lua_tostring(L, 1) << "' AND ";
				}
				if (lua_type(L, 2) != LUA_TNIL)
				{
					query << "`Entities`.`type` LIKE '" << lua_tostring(L, 2) << "' AND ";
				}
				for (int argn = 5; argn <= argc; argn+=2)
				{
					lua_pushnil(L);
					while (lua_next(L, argn) != 0)
					{
						// key at -2
						// value at -1
						
						if (lua_type(L, -2) == LUA_TSTRING)
						{
							std::string column = lua_tostring(L, -2);
							query << "`" << tables[(argn-5)/2] << "`.`" << column << "` ";
							int type = lua_type(L, -1);
							if (type == LUA_TSTRING)
							{
								query << "= '" << lua_tostring(L, -1) << "'";
							}
							else if (type == LUA_TNUMBER)
							{
								query << "= " << lua_tonumber(L, -1);
							}
							else if (type == LUA_TBOOLEAN)
							{
								query << "= " << lua_toboolean(L, -1);
							}
							else if (type == LUA_TTABLE)
							{
								lua_pushnumber(L, 1);
								lua_gettable(L, -2); //now the value, cause we pushed something
								query << ">= " << lua_tonumber(L, -2) << " "; //the value from the inner table
								lua_pop(L, 1); //inner table value popped
								
								lua_pushnumber(L, 2);
								lua_gettable(L, -2); //now the value, cause we pushed something
								query << "AND `" << tables[(argn-5)/2] << "`.`" << column << "` <= " << lua_tonumber(L, -2); //the value from the inner table
								lua_pop(L, 1); //inner table value popped
							}
							query << " AND ";
						}
						
						lua_pop(L, 1); //remove value
					}
				}
				for (unsigned int n = 0;n < tables.size();n++)
				{
					if (n == tables.size()-1)
					{
						query << "`Entities`.`id` = `" << tables[n] << "`.`entity_id`";
					}
					else
					{
						query << "`Entities`.`id` = `" << tables[n] << "`.`entity_id` AND ";
					}
				}
				if (lua_type(L, 3) != LUA_TTABLE)
				{	
					std::vector<std::string> ordercols;
					lua_pushnil(L);
					while (lua_next(L, 3) != 0)
					{
						// key at -2
						// value at -1
						unsigned int key = (unsigned int)lua_tonumber(L, -2);
						std::string val = lua_tostring(L, -1);
						
						while (key >= ordercols.size())
						{
							ordercols.push_back(std::string());
						}
						ordercols[key] = val;
						
						lua_pop(L, 1); //remove value
					}
					
					for (unsigned int n = 0;n < ordercols.size(); n++)
					{
						std::string orderentry = ordercols[n];
						if (orderentry == "")
							continue;
						
						bool asc = true;
						if (orderentry[0] == '-')
						{
							orderentry = orderentry.substr(1);
							asc = false;
						}
						std::vector<std::string> ordersplit = stringsplit(orderentry, ".");
						std::string ordertable = ordersplit[0] + "Components";
						std::string ordercol = ordersplit[1];
						if (n == 0)
							query << " ORDER BY `" << ordertable << "`.`" << ordercol << "`";
						else
							query << ", `" << ordertable << "`.`" << ordercol << "`";
						if (asc)
						{
							query << " ASC";
						}
						else
						{
							query << " DESC";
						}
					}
				}
				else if (lua_type(L, 3) == LUA_TSTRING)
				{
					std::string orderentry = lua_tostring(L, 3);
					bool asc = true;
					if (orderentry[0] == '-')
					{
						orderentry = orderentry.substr(1);
						asc = false;
					}
					std::vector<std::string> ordersplit = stringsplit(orderentry, ".");
					std::string ordertable = ordersplit[0] + "Components";
					std::string ordercol = ordersplit[1];
					query << " ORDER BY `" << ordertable << "`.`" << ordercol << "`";
					if (asc)
					{
						query << " ASC";
					}
					else
					{
						query << " DESC";
					}
				}
				lua_pop(L, argc);
				
				sql::Connection* conn = engine->sql()->connect();
				sql::Statement* stmt = conn->createStatement();
				
				//std::cout << query.str().c_str() << std::endl;
				
				sql::ResultSet* res = stmt->executeQuery(query.str().c_str());
				
				lua_newtable(L);
				int counter = 1;
				while (res->next())
				{
					lua_pushnumber(L, counter);
					pushEntity(L, engine->get_entity(res->getUInt(1)));
					lua_settable(L, 1);
					counter++;
				}
				delete res;
				delete stmt;
				delete conn;
				
				return 1;
			}
			
			int NP_playchar_fcall(lua_State* L)
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
				Engine* engine = static_cast<Engine*>(ud);
				
				std::string charname = lua_tostring(L, 1);
				unsigned int client = (unsigned int)lua_tonumber(L, 2);
				lua_pop(L, 2);
				
				if (!engine->is_pc_name_active(charname))
				{
					bool result = engine->set_pc_name_played_by(charname, client);
					if (result)
					{
						lua_pushnumber(L, 0);
					}
					else
					{
						lua_pushnumber(L, -1);
					}
				}
				else
				{
					//This should be impossible, as users can only login once. Planning for a future possibility.
					lua_pushnumber(L, -2);
				}
				return 1;
			}
			
			int NP_utility_mm_index(lua_State* L)
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
				Proxy* np = static_cast<Proxy*>(ud);
				if (np->type != ProxyEngine)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				std::string key = lua_tostring(L, 2);
				lua_pop(L, 2);
				
				if (key == "noise2d")
				{
					lua_pushcfunction(L, &Util_noise2d);
					
					return 1;
				}
				else if (key == "octave_noise2d")
				{
					lua_pushcfunction(L, &Util_octave_noise2d);
					
					return 1;
				}
				else if (key == "image")
				{
					lua_pushcfunction(L, &Util_image);
					
					return 1;
				}
				
				return 0;
			}
			
			int NP_mm_index(lua_State* L)
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
				Proxy* np = static_cast<Proxy*>(ud);
				if (np->type != ProxyEngine)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				std::string key = lua_tostring(L, 2);
				lua_pop(L, 2);
				
				if (key == "system")
				{
					void* ud2 = lua_newuserdata(L, sizeof(np));
					*static_cast<Proxy*>(ud2) = *np;
					
					//make the metatable
					lua_newtable(L);
					lua_pushstring(L, "__index");
					lua_pushcfunction(L, &NP_system_mm_index);
					lua_settable(L, -3);
					lua_setmetatable(L, -2);
					
					return 1;
				}
				else if (key == "world")
				{
					World* world = np->engine->get_world();
					if (world)
					{
						pushWorld(L, world);
						return 1;
					}
					return 0;
				}
				else if (key == "utility")
				{
					void* ud2 = lua_newuserdata(L, sizeof(np));
					*static_cast<Proxy*>(ud2) = *np;
					
					//make the metatable
					lua_newtable(L);
					lua_pushstring(L, "__index");
					lua_pushcfunction(L, &NP_utility_mm_index);
					lua_settable(L, -3);
					lua_setmetatable(L, -2);
					
					return 1;
				}
				else if (key == "new_entity")
				{
					lua_pushlightuserdata(L, np->engine);
					lua_pushcclosure(L, &NP_new_entity_fcall, 1);
					return 1;
				}
				else if (key == "etc")
				{
					lua_pushlightuserdata(L, np->engine);
					lua_pushcclosure(L, &NP_etc_fcall, 1);
					return 1;
				}
				else if (key == "select")
				{
					lua_pushlightuserdata(L, np->engine);
					lua_pushcclosure(L, &NP_select_fcall, 1);
					return 1;
				}
				else if (key == "playchar")
				{
					lua_pushlightuserdata(L, np->engine);
					lua_pushcclosure(L, &NP_playchar_fcall, 1);
					return 1;
				}
				
				return 0;
			}
			
			void pushEngine(lua_State* L, Engine* engine)
			{
				//make the EngineProxy
				Proxy np;
				np.engine = engine;
				np.type = ProxyEngine;
				
				//make the userdata and copy the EngineProxy into it
				void* ud = lua_newuserdata(L, sizeof(np));
				*static_cast<Proxy*>(ud) = np;
				
				//make the metatable
				lua_newtable(L);
				lua_pushstring(L, "__index");
				lua_pushcfunction(L, &NP_mm_index);
				lua_settable(L, -3);
				lua_setmetatable(L, -2);
			}
		}
	}
}