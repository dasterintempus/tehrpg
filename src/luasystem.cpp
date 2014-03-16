#include "rpgsystem.h"
#include "exceptions.h"

namespace teh
{
	namespace RPG
	{
		namespace Lua
		{
			int SP_subsystem_mm_call(lua_State* L)
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
				Proxy* sp = static_cast<Proxy*>(ud);
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
				int callargc = copyStack(L, 2, other->L());
				
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
				int resultargc = copyStack(other->L(), 1, L);
				
				//Clean up the other stack
				lua_pop(other->L(), lua_gettop(other->L()));
				
				return resultargc;
			}
			
			int SP_mm_index(lua_State* L)
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
				Proxy* sp = static_cast<Proxy*>(ud);
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
				*static_cast<Proxy*>(ud2) = *sp;
				
				//make the metatable
				lua_newtable(L);
				lua_pushstring(L, "__call");
				lua_pushstring(L, funcname.c_str());
				lua_pushcclosure(L, &SP_subsystem_mm_call, 1);
				lua_settable(L, -3);
				lua_setmetatable(L, -2);
				
				return 1;
			}
			
			void pushSystem(lua_State* L, System* system)
			{
				//make the SystemProxy
				Proxy sp;
				sp.system = system;
				sp.type = ProxySystem;
				
				//make the userdata and copy the SystemProxy into it
				void* ud = lua_newuserdata(L, sizeof(sp));
				*static_cast<Proxy*>(ud) = sp;
				
				//make the metatable
				lua_newtable(L);
				lua_pushstring(L, "__index");
				lua_pushcfunction(L, &SP_mm_index);
				lua_settable(L, -3);
				lua_setmetatable(L, -2);
			}
		}
	}
}