#include "rpgsystem.h"
#include "rpgaction.h"
#include "exceptions.h"

namespace teh
{
	namespace RPG
	{
		namespace Lua
		{
			int AP_mm_index(lua_State* L)
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
				Proxy* ap = static_cast<Proxy*>(ud);
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
					pushEntity(L, origin.second);
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
						pushEntity(L, target.second);
						lua_settable(L, -3);
						lua_settable(L, -3);
					}
					return 1;
				}
				return 0;
			}
			
			void pushAction(lua_State* L, Action* action)
			{
				//make the ActionProxy
				Proxy ap;
				ap.action = action;
				ap.type = ProxyAction;
				
				//make the userdata and copy the ActionProxy into it
				void* ud = lua_newuserdata(L, sizeof(ap));
				*static_cast<Proxy*>(ud) = ap;
				
				//make the metatable
				lua_newtable(L);
				lua_pushstring(L, "__index");
				lua_pushcfunction(L, &AP_mm_index);
				lua_settable(L, -3);
				lua_setmetatable(L, -2);
			}
		}
	}
}