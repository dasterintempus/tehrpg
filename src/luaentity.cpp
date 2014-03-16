#include "rpgsystem.h"
#include "rpgentity.h"
#include "rpgengine.h"
#include "rpgaction.h"
#include "rpgcomponent.h"
#include "exceptions.h"

namespace teh
{
	namespace RPG
	{
		namespace Lua
		{
			int EP_component_mm_index(lua_State* L)
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
				Proxy* ep = static_cast<Proxy*>(ud);
				if (ep->type != ProxyEntity)
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
				
				Component* component = ep->entity->component(key);
				if (component)
				{
					pushComponent(L, component);
					return 1;
				}
				
				return 0;
			}
			
			int EP_act_fcall(lua_State* L)
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
				Proxy* np = static_cast<Proxy*>(lua_touserdata(L, -1));
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
					Proxy* ep = static_cast<Proxy*>(ud);
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
			
			int EP_msg_fcall(lua_State* L)
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
				Proxy* np = static_cast<Proxy*>(lua_touserdata(L, -1));
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
			
			int EP_attach_fcall(lua_State* L)
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
				
				std::string componenttype = lua_tostring(L, 1);
				lua_pop(L, 1);
				
				lua_getglobal(L, "engine");
				Proxy* np = static_cast<Proxy*>(lua_touserdata(L, -1));
				if (np->type != ProxyEngine)
				{
					//TODO: Error?
					lua_pop(L, argc+1);
					return 0;
				}
				Engine* engine = np->engine;
				lua_pop(L, 1);
				
				Component* c = new Component(componenttype, engine);
				int result = c->attach(entity);
				if (result != 0)
				{
					delete c;
					lua_pushnil(L);
					lua_pushnumber(L, result);
					return 2;
				}
				
				pushComponent(L, c);
				
				return 1;
			}
			
			int EP_detach_fcall(lua_State* L)
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
				
				std::string componenttype = lua_tostring(L, 1);
				lua_pop(L, 1);
				
				entity->detach(componenttype);

				return 0;
			}
			
			int EP_destroy_fcall(lua_State* L)
			{
				int argc = lua_gettop(L);
				if (argc != 0)
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
				
				entity->destroy();
				
				return 0;
			}
			
			int EP_setuserid_fcall(lua_State* L)
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
				
				entity->userid((unsigned int)lua_tonumber(L, 1));
				lua_pop(L, 1);
				
				return 0;
			}
			
			int EP_mm_index(lua_State* L)
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
				Proxy* ep = static_cast<Proxy*>(ud);
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
					*static_cast<Proxy*>(ud2) = *ep;
					
					//make the metatable
					lua_newtable(L);
					lua_pushstring(L, "__index");
					lua_pushcfunction(L, &EP_component_mm_index);
					lua_settable(L, -3);
					lua_setmetatable(L, -2);
					return 1;
				}
				else if (key == "act")
				{
					lua_pushlightuserdata(L, ep->entity);
					lua_pushcclosure(L, &EP_act_fcall, 1);
					return 1;
				}
				else if (key == "msg")
				{
					lua_pushlightuserdata(L, ep->entity);
					lua_pushcclosure(L, &EP_msg_fcall, 1);
					return 1;
				}
				else if (key == "attach")
				{
					lua_pushlightuserdata(L, ep->entity);
					lua_pushcclosure(L, &EP_attach_fcall, 1);
					return 1;
				}
				else if (key == "detach")
				{
					lua_pushlightuserdata(L, ep->entity);
					lua_pushcclosure(L, &EP_detach_fcall, 1);
					return 1;
				}
				else if (key == "destroy")
				{
					lua_pushlightuserdata(L, ep->entity);
					lua_pushcclosure(L, &EP_destroy_fcall, 1);
					return 1;
				}
				else if (key == "setuserid")
				{
					lua_pushlightuserdata(L, ep->entity);
					lua_pushcclosure(L, &EP_setuserid_fcall, 1);
					return 1;
				}
				
				return 0;
			}
			
			int EP_mm_eq(lua_State* L)
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
				Proxy* ep1 = static_cast<Proxy*>(ud1);
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
				Proxy* ep2 = static_cast<Proxy*>(ud2);
				if (ep2->type != ProxyEntity)
				{
					//TODO: Error?
					lua_pop(L, argc);
					return 0;
				}
				
				lua_pushboolean(L, ep1->entity->id() == ep2->entity->id());
				return 1;
			}
			
			void pushEntity(lua_State* L, Entity* entity)
			{
				//make the EntityProxy
				Proxy ep;
				ep.entity = entity;
				ep.type = ProxyEntity;
				//ep.engine = engine;
				
				//make the userdata and copy the EntityProxy into it
				void* ud = lua_newuserdata(L, sizeof(ep));
				*static_cast<Proxy*>(ud) = ep;
				
				//make the metatable
				lua_newtable(L);
				lua_pushstring(L, "__index");
				lua_pushcfunction(L, &EP_mm_index);
				lua_settable(L, -3);
				lua_pushstring(L, "__eq");
				lua_pushcfunction(L, &EP_mm_eq);
				lua_settable(L, -3);
				//lua_pushstring(L, "__newindex");
				//lua_pushcfunction(L, &EP_mm_newindex);
				//lua_settable(L, -3);
				lua_setmetatable(L, -2);
			}
		}
	}
}