#include "rpgsystem.h"
#include "rpgengine.h"
#include "rpgcomponent.h"
#include "rpgentity.h"
#include "exceptions.h"

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
				lua_pop(L, 2);
				return 0;
			}
			LuaProxy* cp = static_cast<LuaProxy*>(ud);
			if (cp->type != ProxyComponent)
			{
				//TODO: Error?
				lua_pop(L, 2);
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
			}
			else if (vt == Component::TypeUInt)
			{
				lua_pushnumber(L, cp->component->getUInt(key));
			}
			else if (vt == Component::TypeBool)
			{
				lua_pushboolean(L, cp->component->getBool(key));
			}
			else if (vt == Component::TypeString)
			{
				lua_pushstring(L, cp->component->getString(key).c_str());
			}
			else
			{
				return 0;
			}
			return 1;
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
				lua_pop(L, 2);
				return 0;
			}
			LuaProxy* cp = static_cast<LuaProxy*>(ud);
			if (cp->type != ProxyComponent)
			{
				//TODO: Error?
				lua_pop(L, 2);
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
				lua_pop(L, 2);
				return 0;
			}
			LuaProxy* ep = static_cast<LuaProxy*>(ud);
			if (ep->type != ProxyEntity)
			{
				//TODO: Error?
				lua_pop(L, 2);
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
				lua_pop(L, 2);
				return 0;
			}
			LuaProxy* ep = static_cast<LuaProxy*>(ud);
			if (ep->type != ProxyEntity)
			{
				//TODO: Error?
				lua_pop(L, 2);
				return 0;
			}
			std::string key = lua_tostring(L, 2);
			lua_pop(L, 2);
			
			if (key == "name")
			{
				lua_pushstring(L, ep->entity->name().c_str());
			}
			else if (key == "type")
			{
				lua_pushstring(L, ep->entity->type().c_str());
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
			}
			else
			{
				return 0;
			}
			
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
				lua_pop(L, 2);
				return 0;
			}
			LuaProxy* np = static_cast<LuaProxy*>(ud);
			if (np->type != ProxyEngine)
			{
				//TODO: Error?
				lua_pop(L, 2);
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
				lua_pop(L, 2);
				return 0;
			}
			LuaProxy* np = static_cast<LuaProxy*>(ud);
			if (np->type != ProxyEngine)
			{
				//TODO: Error?
				lua_pop(L, 2);
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
			}
			else
			{
				return 0;
			}
			
			return 1;
		}
		
		int LuaSP_mm_call(lua_State* L)
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
			
			//push arguments to other->L()
			int callargc = System::copyStack(L, 3, other->L());
			lua_pop(L, 2);
			
			lua_call(other->L(), callargc, LUA_MULTRET);
			
			//pull arguments back
			int resultargc = System::copyStack(other->L(), 1, L);
			return resultargc;
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
		
		std::string System::process(Entity* entity)
		{
			lua_getglobal(_L, "process");
			if (lua_isnil(_L, -1))
			{
				lua_pop(_L, 1);
				return "";
			}

			pushEntity(_L, entity);

			lua_call(_L, 1, 1);
			
			return lua_tostring(_L, -1);
		}
		
		void System::initFunctions()
		{
			pushEngine(L(), _engine);
			lua_setglobal(L(), "engine");
			pushSystem(L(), this);
			lua_setglobal(L(), "this");
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
			lua_pushstring(L, "__call");
			lua_pushcfunction(L, &LuaSP_mm_call);
			lua_settable(L, -3);
			lua_setmetatable(L, -2);
		}
		
		int System::copyStack(lua_State* sourceL, int startindex, lua_State* destL)
		{
			int argc = lua_gettop(sourceL);
			std::cout << "copying stack of size " << argc << std::endl;
			int output = 0;
			void* received_ud = 0;
			LuaProxy* received = 0;
			for (unsigned int n = startindex; n <= argc; n++)
			{
				switch (lua_type(sourceL, -1))
				{
					case LUA_TNUMBER:
						lua_pushnumber(destL, lua_tonumber(sourceL, -1));
						output++;
						break;
					case LUA_TSTRING:
						lua_pushstring(destL, lua_tostring(sourceL, -1));
						output++;
						break;
					case LUA_TBOOLEAN:
						lua_pushboolean(destL, lua_toboolean(sourceL, -1));
						output++;
						break;
					case LUA_TNIL:
						lua_pushnil(destL);
						output++;
						break;
					case LUA_TUSERDATA:
						std::cout << "Copying userdata of type ";
						received_ud = lua_touserdata(sourceL, -1);
						received = static_cast<LuaProxy*>(received_ud);
						if (received->type == ProxyComponent)
						{
							std::cout << "Component" << std::endl;
							System::pushComponent(destL, received->component);
							output++;
						}
						else if (received->type == ProxyEntity)
						{
							std::cout << "Entity" << std::endl;
							System::pushEntity(destL, received->entity);
							output++;
						}
						else if (received->type == ProxyEngine)
						{
							std::cout << "Engine" << std::endl;
							System::pushEngine(destL, received->engine);
							output++;
						}
						else if (received->type == ProxySystem)
						{
							std::cout << "System" << std::endl;
							System::pushSystem(destL, received->system);
							output++;
						}
						break;
					default:
						break;
				}
				lua_pop(sourceL, 1);
			}
			return output;
		}
	}
}