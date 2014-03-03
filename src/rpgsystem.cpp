#include "rpgsystem.h"
#include "rpgengine.h"
#include "rpgcomponent.h"
#include "rpgentity.h"
#include "rpgaction.h"
#include "exceptions.h"
#include "rpgentityfactory.h"

#include <iostream>

namespace teh
{
	namespace RPG
	{
		int LuaCP_mm_index(lua_State* L)
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
			LuaProxy* cp = static_cast<LuaProxy*>(ud);
			if (cp->type != ProxyComponent)
			{
				//TODO: Error?
				lua_pop(L, argc);
				return 0;
			}
			std::string key = lua_tostring(L, 2);
			lua_pop(L, 2);
			
			lua_getglobal(L, "engine");
			LuaProxy* np = static_cast<LuaProxy*>(lua_touserdata(L, -1));
			lua_pop(L, 1);
			if (np->type != ProxyEngine)
			{
				//TODO: Error?
				return 0;
			}
			Engine* engine = np->engine;
			
			//Get schema
			Component::ValueType vt = Component::getSchemaKeyType(cp->component->componenttype(), engine, key);
			if (vt == Component::TypeInt)
			{
				lua_pushnumber(L, cp->component->getInt(key));
				return 1;
			}
			else if (vt == Component::TypeUInt)
			{
				lua_pushnumber(L, cp->component->getUInt(key));
				return 1;
			}
			else if (vt == Component::TypeBool)
			{
				lua_pushboolean(L, cp->component->getBool(key));
				return 1;
			}
			else if (vt == Component::TypeString)
			{
				lua_pushstring(L, cp->component->getString(key).c_str());
				return 1;
			}
			
			return 0;
		}
		
		int LuaCP_mm_newindex(lua_State* L)
		{
			int argc = lua_gettop(L);
			if (argc != 3)
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
			LuaProxy* cp = static_cast<LuaProxy*>(ud);
			if (cp->type != ProxyComponent)
			{
				//TODO: Error?
				lua_pop(L, argc);
				return 0;
			}
			std::string key = lua_tostring(L, 2);
			
			lua_getglobal(L, "engine");
			LuaProxy* np = static_cast<LuaProxy*>(lua_touserdata(L, -1));
			lua_pop(L, 1);
			if (np->type != ProxyEngine)
			{
				//TODO: Error?
				return 0;
			}
			Engine* engine = np->engine;
			
			//Get schema
			Component::ValueType vt = Component::getSchemaKeyType(cp->component->componenttype(), engine, key);
			
			if (vt == Component::TypeInt)
			{
				long int value = (long int)lua_tonumber(L, 3);
				cp->component->setInt(key, value);
			}
			else if (vt == Component::TypeUInt)
			{
				unsigned int value = (unsigned int)lua_tonumber(L, 3);
				cp->component->setUInt(key, value);
			}
			else if (vt == Component::TypeBool)
			{
				bool value = lua_toboolean(L, 3);
				cp->component->setBool(key, value);
			}
			else if (vt == Component::TypeString)
			{
				std::string value = lua_tostring(L, 3);
				cp->component->setString(key, value);
			}
			else
			{
				lua_pop(L, 3);
				return 0;
			}
			lua_pop(L, 3);
			return 0;
		}
		
		int LuaEP_component_mm_index(lua_State* L)
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
			LuaProxy* ep = static_cast<LuaProxy*>(ud);
			if (ep->type != ProxyEntity)
			{
				//TODO: Error?
				lua_pop(L, argc);
				return 0;
			}
			std::string key = lua_tostring(L, 2);
			lua_pop(L, 2);
			
			lua_getglobal(L, "engine");
			LuaProxy* np = static_cast<LuaProxy*>(lua_touserdata(L, -1));
			lua_pop(L, 1);
			if (np->type != ProxyEngine)
			{
				//TODO: Error?
				return 0;
			}
			Engine* engine = np->engine;
			
			Component* component = ep->entity->component(key);
			if (component)
			{
				System::pushComponent(L, component);
				return 1;
			}
			
			return 0;
		}
		
		int LuaEP_act_fcall(lua_State* L)
		{
			int argc = lua_gettop(L);
			if (argc%2 != 1) //Needs odd number of arguments
			{
				//TODO: Error?
				lua_pop(L, argc);
				return 0;
			}
			void* ptr = lua_touserdata(L, lua_upvalueindex(1));
			if (!ptr)
			{
				//TODO: Error?
				lua_pop(L, argc);
				return 0;
			}
			Entity* entity = static_cast<Entity*>(ptr);
			
			lua_getglobal(L, "engine");
			LuaProxy* np = static_cast<LuaProxy*>(lua_touserdata(L, -1));
			//lua_pop(L, 1);
			if (np->type != ProxyEngine)
			{
				//TODO: Error?
				lua_pop(L, argc+1);
				return 0;
			}
			Engine* engine = np->engine;
			
			std::string verb = lua_tostring(L, 1);
			Action::Actor origin = std::make_pair(verb, entity);
			std::vector<Action::Actor> targets;
			for (unsigned int n = 2; n < argc; n+=2)
			{
				std::string nverb = lua_tostring(L, n);
				void* ud = lua_touserdata(L, n+1);
				if (!ud)
				{
					std::cerr << "Unable to get userdata for act()" << std::endl;
					//TODO: Error?
					lua_pop(L, argc+1);
					return 0;
				}
				LuaProxy* ep = static_cast<LuaProxy*>(ud);
				if (ep->type != ProxyEntity)
				{
					std::cerr << "userdata is not ProxyEntity for act()" << std::endl;
					//TODO: Error?
					lua_pop(L, argc+1);
					return 0;
				}
				Entity* nentity = ep->entity;
				Action::Actor target = std::make_pair(nverb, nentity);
				targets.push_back(target);
			}
			lua_pop(L, argc+1);
			
			Action* action = new Action(origin, targets);
			engine->queue_action(entity->id(), action);
			return 0;
		}
		
		int LuaEP_msg_fcall(lua_State* L)
		{
			int argc = lua_gettop(L);
			if (argc != 1)
			{
				//TODO: Error?
				lua_pop(L, argc);
				return 0;
			}
			void* ptr = lua_touserdata(L, lua_upvalueindex(1));
			if (!ptr)
			{
				//TODO: Error?
				lua_pop(L, argc);
				return 0;
			}
			Entity* entity = static_cast<Entity*>(ptr);
			std::string msg = lua_tostring(L, 1);
			
			lua_getglobal(L, "engine");
			LuaProxy* np = static_cast<LuaProxy*>(lua_touserdata(L, -1));
			//lua_pop(L, 1);
			if (np->type != ProxyEngine)
			{
				//TODO: Error?
				lua_pop(L, 2);
				return 0;
			}
			Engine* engine = np->engine;
			
			lua_pop(L, 2);
			
			engine->message_entity(entity, msg);
			
			return 0;
		}
		
		int LuaEP_mm_index(lua_State* L)
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
			LuaProxy* ep = static_cast<LuaProxy*>(ud);
			if (ep->type != ProxyEntity)
			{
				//TODO: Error?
				lua_pop(L, argc);
				return 0;
			}
			std::string key = lua_tostring(L, 2);
			lua_pop(L, 2);
			
			if (key == "name")
			{
				lua_pushstring(L, ep->entity->name().c_str());
				return 1;
			}
			else if (key == "type")
			{
				lua_pushstring(L, ep->entity->type().c_str());
				return 1;
			}
			else if (key == "component")
			{
				void* ud2 = lua_newuserdata(L, sizeof(ep));
				*static_cast<LuaProxy*>(ud2) = *ep;
				
				//make the metatable
				lua_newtable(L);
				lua_pushstring(L, "__index");
				lua_pushcfunction(L, &LuaEP_component_mm_index);
				lua_settable(L, -3);
				lua_setmetatable(L, -2);
				return 1;
			}
			else if (key == "act")
			{
				lua_pushlightuserdata(L, ep->entity);
				lua_pushcclosure(L, &LuaEP_act_fcall, 1);
				return 1;
			}
			else if (key == "msg")
			{
				lua_pushlightuserdata(L, ep->entity);
				lua_pushcclosure(L, &LuaEP_msg_fcall, 1);
				return 1;
			}
			
			return 0;
		}
		
		int LuaEP_mm_eq(lua_State* L)
		{
			int argc = lua_gettop(L);
			if (argc != 2)
			{
				//TODO: Error?
				lua_pop(L, argc);
				return 0;
			}
			void* ud1 = lua_touserdata(L, 1);
			if (!ud1)
			{
				//TODO: Error?
				lua_pop(L, argc);
				return 0;
			}
			LuaProxy* ep1 = static_cast<LuaProxy*>(ud1);
			if (ep1->type != ProxyEntity)
			{
				//TODO: Error?
				lua_pop(L, argc);
				return 0;
			}
			void* ud2 = lua_touserdata(L, 2);
			if (!ud2)
			{
				//TODO: Error?
				lua_pop(L, argc);
				return 0;
			}
			LuaProxy* ep2 = static_cast<LuaProxy*>(ud2);
			if (ep2->type != ProxyEntity)
			{
				//TODO: Error?
				lua_pop(L, argc);
				return 0;
			}
			
			lua_pushboolean(L, ep1->entity->id() == ep2->entity->id());
			return 1;
		}
		
		int LuaNP_system_mm_index(lua_State* L)
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
			LuaProxy* np = static_cast<LuaProxy*>(ud);
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
				System::pushSystem(L, system);
				return 1;
			}
			return 0;
		}
		
		int LuaNP_tile_at_fcall(lua_State* L)
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
			
			int xpos = (int)lua_tonumber(L, 1);
			int ypos = (int)lua_tonumber(L, 2);
			lua_pop(L, 2);
			
			Entity* tile = findTile(engine, xpos, ypos);
			if (tile)
			{
				System::pushEntity(L, tile);
				return 1;
			}
			
			return 0;
		}
		
		int LuaNP_characters_at_fcall(lua_State* L)
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
			
			int xpos = (int)lua_tonumber(L, 1);
			int ypos = (int)lua_tonumber(L, 2);
			lua_pop(L, 2);
			
			std::vector<Entity*> characters = findCharactersAt(engine, xpos, ypos);
			std::cerr << "Characters vector size: " << characters.size() << std::endl;
			std::cerr << "Coord: " << xpos << ", " << ypos << std::endl;
			lua_newtable(L);
			for (unsigned int n = 0;n < characters.size(); n++)
			{
				lua_pushnumber(L, n+1);
				System::pushEntity(L, characters[n]);
				lua_settable(L, -3);
			}
			return 1;
		}
		
		int LuaNP_mm_index(lua_State* L)
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
			LuaProxy* np = static_cast<LuaProxy*>(ud);
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
				*static_cast<LuaProxy*>(ud2) = *np;
				
				//make the metatable
				lua_newtable(L);
				lua_pushstring(L, "__index");
				lua_pushcfunction(L, &LuaNP_system_mm_index);
				lua_settable(L, -3);
				lua_setmetatable(L, -2);
				
				return 1;
			}
			else if (key == "tile_at")
			{
				lua_pushlightuserdata(L, np->engine);
				lua_pushcclosure(L, &LuaNP_tile_at_fcall, 1);
				
				return 1;
			}
			else if (key == "characters_at")
			{
				lua_pushlightuserdata(L, np->engine);
				lua_pushcclosure(L, &LuaNP_characters_at_fcall, 1);
				
				return 1;
			}
			
			return 0;
		}
		
		int LuaSP_subsystem_mm_call(lua_State* L)
		{
			int argc = lua_gettop(L);
			if (argc < 1)
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
			LuaProxy* sp = static_cast<LuaProxy*>(ud);
			if (sp->type != ProxySystem)
			{
				//TODO: Error?
				lua_pop(L, argc);
				return 0;
			}
			
			std::string funcname = lua_tostring(L, lua_upvalueindex(1));
			System* other = sp->system;
			
			//Get the backtracer on other-L()
			lua_getglobal(other->L(), "_backtracer");
			
			std::cerr << "looking for function: " << funcname << std::endl;
			
			lua_getglobal(other->L(), funcname.c_str());
			if (lua_isnil(other->L(), -1))
			{
				lua_pop(other->L(), 2);
				lua_pop(L, argc);
				return 0;
			}
			
			//push arguments to other->L()
			int callargc = System::copyStack(L, 2, other->L());
			
			//Clean up this stack
			lua_pop(L, argc);
			
			int callresult = lua_pcall(other->L(), callargc, LUA_MULTRET, 1);
			if (callresult)
			{
				if (callresult == LUA_ERRRUN)
				{
					std::cerr << lua_tostring(other->L(), -1);
					lua_pop(other->L(), lua_gettop(other->L()));
					return 0;
				}
				else if (callresult == LUA_ERRERR)
				{
					std::cerr << "Lua error handler error" << std::endl;
					return 0;
				}
			}
			
			lua_remove(other->L(), 1); // remove the backtracer
			
			//pull arguments back
			int resultargc = System::copyStack(other->L(), 1, L);
			
			//Clean up the other stack
			lua_pop(other->L(), lua_gettop(other->L()));
			
			return resultargc;
		}
		
		int LuaSP_mm_index(lua_State* L)
		{
			int argc = lua_gettop(L);
			if (argc < 2)
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
			LuaProxy* sp = static_cast<LuaProxy*>(ud);
			if (sp->type != ProxySystem)
			{
				//TODO: Error?
				lua_pop(L, argc);
				return 0;
			}
			std::string funcname = lua_tostring(L, 2);
			System* other = sp->system;
			
			lua_getglobal(other->L(), funcname.c_str());
			if (lua_isnil(other->L(), -1))
			{
				lua_pop(other->L(), 1);
				return 0;
			}
			lua_pop(other->L(), 1);
			
			void* ud2 = lua_newuserdata(L, sizeof(sp));
			*static_cast<LuaProxy*>(ud2) = *sp;
			
			//make the metatable
			lua_newtable(L);
			lua_pushstring(L, "__call");
			lua_pushstring(L, funcname.c_str());
			lua_pushcclosure(L, &LuaSP_subsystem_mm_call, 1);
			lua_settable(L, -3);
			lua_setmetatable(L, -2);
			
			return 1;
		}
		
		int LuaAP_mm_index(lua_State* L)
		{
			int argc = lua_gettop(L);
			if (argc < 2)
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
			LuaProxy* ap = static_cast<LuaProxy*>(ud);
			if (ap->type != ProxyAction)
			{
				//TODO: Error?
				lua_pop(L, argc);
				return 0;
			}
			
			std::string key = lua_tostring(L, 2);
			lua_pop(L, 2);
			Action* action = ap->action;
			
			if (key == "origin")
			{
				Action::Actor origin = action->origin();
				
				lua_newtable(L);
				lua_pushstring(L, "verb");
				lua_pushstring(L, origin.first.c_str());
				lua_settable(L, -3);
				lua_pushstring(L, "entity");
				System::pushEntity(L, origin.second);
				lua_settable(L, -3);
				return 1;
			}
			else if (key == "targets")
			{
				std::vector<Action::Actor> targets = action->targets();
				
				lua_newtable(L);
				for (unsigned int n=0;n<targets.size();n++)
				{
					Action::Actor target = targets[n];
					lua_pushnumber(L, n+1);
					lua_newtable(L);
					lua_pushstring(L, "verb");
					lua_pushstring(L, target.first.c_str());
					lua_settable(L, -3);
					lua_pushstring(L, "entity");
					System::pushEntity(L, target.second);
					lua_settable(L, -3);
					lua_settable(L, -3);
				}
				return 1;
			}
			return 0;
		}
		
		int LuaDebug_backtrace(lua_State* L)
		{
			std::stringstream msg;
			msg << "Lua error: " << lua_tostring(L, 1) << std::endl;
			lua_pop(L, 1);
			lua_Debug debug;
			int stackdepth = 0;
			while (lua_getstack(L, stackdepth, &debug))
			{
				lua_getinfo(L, "nSl", &debug);
				if (debug.name)
				{
					msg << "Line: " << debug.currentline << " in function " << debug.name << " in file " << debug.short_src << " at stack depth " << stackdepth << std::endl;
				}
				else
				{
					msg << "Line: " << debug.currentline << " in function " << "UNDETERMINED" << " in file " << debug.short_src << " at stack depth " << stackdepth << std::endl;
				}
				
				stackdepth++;
			}
			lua_pushstring(L, msg.str().c_str());
			std::cout << msg.str();
			return 1;
		}
		
		System::System(const std::string& name, Engine* engine)
			: _name(name), _engine(engine), _L(0)
		{
			std::string luafilename = "./lua/" + name + ".lua";
			_L = luaL_newstate();
			luaL_openlibs(_L);
			initFunctions();
			if (luaL_dofile(_L, luafilename.c_str()))
				throw teh::Exceptions::InvalidSystem(name);
		}
		
		System::~System()
		{
			lua_close(_L);
		}
		
		std::string System::process_command(Entity* entity, const Command& cmd, bool& ok)
		{
			lua_getglobal(L(), "_backtracer");
			
			lua_getglobal(L(), "process_command");
			if (lua_isnil(L(), -1))
			{
				lua_pop(L(), 2);
				return "";
			}
			
			pushEntity(L(), entity);
			System::pushStringVector(L(), cmd.arguments);

			int result = lua_pcall(L(), 2, 2, 1);
			if (result)
			{
				if (result == LUA_ERRRUN)
				{
					std::cerr << "Lua runtime error: " << std::endl;
					std::cerr << lua_tostring(L(), -1);
					lua_pop(L(), lua_gettop(L()));
					ok = false;
					return "";
				}
				else if (result == LUA_ERRERR)
				{
					std::cerr << "Lua error handler error" << std::endl;
					ok = false;
					return "";
				}
			}
			
			ok = lua_toboolean(L(), -2);
			std::string output = lua_tostring(L(), -1);
			lua_pop(L(), 3);
			
			return output;
		}
		
		int System::process_action(Action* action)
		{
			lua_getglobal(L(), "_backtracer");
			
			lua_getglobal(L(), "process_action");
			if (lua_isnil(L(), -1))
			{
				lua_pop(L(), 2);
				return -1;
			}
			
			pushAction(L(), action);
			
			int result = lua_pcall(L(), 1, 1, 1);
			if (result)
			{
				if (result == LUA_ERRRUN)
				{
					std::cerr << "Lua runtime error: " << std::endl;
					std::cerr << lua_tostring(L(), -1);
					lua_pop(L(), lua_gettop(L()));
					return -1;
				}
				else if (result == LUA_ERRERR)
				{
					std::cerr << "Lua error handler error" << std::endl;
					return -1;
				}
			}
			
			int output = (int)lua_tonumber(L(), -1);
			lua_pop(L(), 2);
			
			return output;
		}
		
		void System::initFunctions()
		{
			pushEngine(L(), _engine);
			lua_setglobal(L(), "engine");
			lua_pushcfunction(L(), LuaDebug_backtrace);
			lua_setglobal(L(), "_backtracer");
			//pushSystem(L(), this);
			//lua_setglobal(L(), "this");
		}
		
		lua_State* System::L()
		{
			return _L;
		}
		
		void System::pushComponent(lua_State* L, Component* component)
		{
			//make the ComponentProxy
			LuaProxy cp;
			cp.component = component;
			cp.type = ProxyComponent;
			//cp.engine = engine;
			
			//make the userdata and copy the ComponentProxy into it
			void* ud = lua_newuserdata(L, sizeof(cp));
			*static_cast<LuaProxy*>(ud) = cp;
			
			//make the metatable
			lua_newtable(L);
			lua_pushstring(L, "__index");
			lua_pushcfunction(L, &LuaCP_mm_index);
			lua_settable(L, -3);
			lua_pushstring(L, "__newindex");
			lua_pushcfunction(L, &LuaCP_mm_newindex);
			lua_settable(L, -3);
			lua_setmetatable(L, -2);
		}
		
		void System::pushEntity(lua_State* L, Entity* entity)
		{
			//make the EntityProxy
			LuaProxy ep;
			ep.entity = entity;
			ep.type = ProxyEntity;
			//ep.engine = engine;
			
			//make the userdata and copy the EntityProxy into it
			void* ud = lua_newuserdata(L, sizeof(ep));
			*static_cast<LuaProxy*>(ud) = ep;
			
			//make the metatable
			lua_newtable(L);
			lua_pushstring(L, "__index");
			lua_pushcfunction(L, &LuaEP_mm_index);
			lua_settable(L, -3);
			lua_pushstring(L, "__eq");
			lua_pushcfunction(L, &LuaEP_mm_eq);
			lua_settable(L, -3);
			//lua_pushstring(L, "__newindex");
			//lua_pushcfunction(L, &LuaEP_mm_newindex);
			//lua_settable(L, -3);
			lua_setmetatable(L, -2);
		}
		
		void System::pushEngine(lua_State* L, Engine* engine)
		{
			//make the EngineProxy
			LuaProxy np;
			np.engine = engine;
			np.type = ProxyEngine;
			
			//make the userdata and copy the EngineProxy into it
			void* ud = lua_newuserdata(L, sizeof(np));
			*static_cast<LuaProxy*>(ud) = np;
			
			//make the metatable
			lua_newtable(L);
			lua_pushstring(L, "__index");
			lua_pushcfunction(L, &LuaNP_mm_index);
			lua_settable(L, -3);
			lua_setmetatable(L, -2);
		}
		
		void System::pushSystem(lua_State* L, System* system)
		{
			//make the SystemProxy
			LuaProxy sp;
			sp.system = system;
			sp.type = ProxySystem;
			
			//make the userdata and copy the SystemProxy into it
			void* ud = lua_newuserdata(L, sizeof(sp));
			*static_cast<LuaProxy*>(ud) = sp;
			
			//make the metatable
			lua_newtable(L);
			lua_pushstring(L, "__index");
			lua_pushcfunction(L, &LuaSP_mm_index);
			lua_settable(L, -3);
			lua_setmetatable(L, -2);
		}
		
		void System::pushAction(lua_State* L, Action* action)
		{
			//make the ActionProxy
			LuaProxy ap;
			ap.action = action;
			ap.type = ProxyAction;
			
			//make the userdata and copy the ActionProxy into it
			void* ud = lua_newuserdata(L, sizeof(ap));
			*static_cast<LuaProxy*>(ud) = ap;
			
			//make the metatable
			lua_newtable(L);
			lua_pushstring(L, "__index");
			lua_pushcfunction(L, &LuaAP_mm_index);
			lua_settable(L, -3);
			lua_setmetatable(L, -2);
		}
		
		void System::pushStringVector(lua_State* L, const std::vector<std::string>& vec)
		{
			lua_newtable(L);
			for (unsigned int n = 0; n < vec.size(); n++)
			{
				lua_pushnumber(L, n+1);
				lua_pushstring(L, vec[n].c_str());
				lua_settable(L, -3);
			}
		}
		
		bool System::copyStackValue(lua_State* sourceL, int index, lua_State* destL)
		{
			void* received_ud = 0;
			LuaProxy* received = 0;
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
						if (System::copyStackValue(sourceL, -2, destL))
						{
							//Check that it really copied the key
							//Now push value to dest
							if (System::copyStackValue(sourceL, -1, destL))
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
					received = static_cast<LuaProxy*>(received_ud);
					if (received->type == ProxyComponent)
					{
						std::cerr << "Component" << std::endl;
						System::pushComponent(destL, received->component);
						pushed = true;
					}
					else if (received->type == ProxyEntity)
					{
						std::cerr << "Entity" << std::endl;
						System::pushEntity(destL, received->entity);
						pushed = true;
					}
					else if (received->type == ProxyEngine)
					{
						std::cerr << "Engine" << std::endl;
						System::pushEngine(destL, received->engine);
						pushed = true;
					}
					else if (received->type == ProxySystem)
					{
						std::cerr << "System" << std::endl;
						System::pushSystem(destL, received->system);
						pushed = true;
					}
					break;
				default:
					break;
			}
			//lua_pop(sourceL, 1);
			return pushed;
		}
		
		int System::copyStack(lua_State* sourceL, int startindex, lua_State* destL)
		{
			int argc = lua_gettop(sourceL);
			std::cerr << "copying stack of size " << argc << " starting at " << startindex << std::endl;
			int output = 0;

			for (unsigned int n = startindex; n <= argc; n++)
			{
				if (System::copyStackValue(sourceL, n, destL))
					output++;
				//lua_pop(sourceL, 1);
			}
			return output;
		}
	}
}