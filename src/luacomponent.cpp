#include "rpgsystem.h"
#include "rpgcomponent.h"
#include "exceptions.h"

namespace teh
{
	namespace RPG
	{
		namespace Lua
		{
			int CP_mm_index(lua_State* L)
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
				Proxy* cp = static_cast<Proxy*>(ud);
				if (cp->type != ProxyComponent)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				std::string key = lua_tostring(L, 2);
				lua_pop(L, 2);
				
				lua_getglobal(L, "engine");
				Proxy* np = static_cast<Proxy*>(lua_touserdata(L, -1));
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
			
			int CP_mm_newindex(lua_State* L)
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
				Proxy* cp = static_cast<Proxy*>(ud);
				if (cp->type != ProxyComponent)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				std::string key = lua_tostring(L, 2);
				
				lua_getglobal(L, "engine");
				Proxy* np = static_cast<Proxy*>(lua_touserdata(L, -1));
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
				lua_pop(L, 3);
				return 0;
			}
			
			void pushComponent(lua_State* L, Component* component)
			{
				//make the ComponentProxy
				Proxy cp;
				cp.component = component;
				cp.type = ProxyComponent;
				//cp.engine = engine;
				
				//make the userdata and copy the ComponentProxy into it
				void* ud = lua_newuserdata(L, sizeof(cp));
				*static_cast<Proxy*>(ud) = cp;
				
				//make the metatable
				lua_newtable(L);
				lua_pushstring(L, "__index");
				lua_pushcfunction(L, &CP_mm_index);
				lua_settable(L, -3);
				lua_pushstring(L, "__newindex");
				lua_pushcfunction(L, &CP_mm_newindex);
				lua_settable(L, -3);
				lua_setmetatable(L, -2);
			}
		}
	}
}