#include "rpgsystem.h"
#include "exceptions.h"
#include "rpgengine.h"
#include "gameserver.h"

#include <iostream>

namespace teh
{
	namespace RPG
	{
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
			//std::cout << msg.str();
			return 1;
		}
		
		System::System(const std::string& name, Engine* engine)
			: _name(name), _engine(engine), _L(0)
		{
			std::string luafilename = _engine->get_module_path() + "script/" + name + ".lua";
			_L = luaL_newstate();
			luaL_openlibs(_L);
			initFunctions();
			
			//Call the file with the backtracer enabled
			lua_getglobal(_L, "_backtracer");
			
			int fileresult = luaL_loadfile(_L, luafilename.c_str());
			if (fileresult == LUA_ERRFILE)
				throw teh::Exceptions::InvalidSystem(name);
			else if (fileresult == LUA_ERRSYNTAX)
			{
				std::stringstream msg;
				msg << "Lua syntax error: " << std::endl;
				msg << lua_tostring(L(), -1);
				lua_pop(L(), lua_gettop(L()));
				throw teh::Exceptions::SystemLuaError(msg.str());
			}
			
			int result = lua_pcall(_L, 0, LUA_MULTRET, 1);
			if (result)
			{
				if (result == LUA_ERRRUN)
				{
					std::stringstream msg;
					msg << "Lua runtime error: " << std::endl;
					msg << lua_tostring(L(), -1);
					lua_pop(L(), lua_gettop(L()));
					throw teh::Exceptions::SystemLuaError(msg.str());
				}
				else if (result == LUA_ERRERR)
				{
					std::stringstream msg;
					msg << "Lua error handler error" << std::endl;
					throw teh::Exceptions::SystemLuaError(msg.str());
				}
			}
			lua_pop(_L, 1);
		}
		
		System::~System()
		{
			lua_close(_L);
		}
		
		std::string System::process_command(Entity* entity, const Command& cmd, const std::string& fname, bool& ok)
		{
			lua_getglobal(L(), "_backtracer");
			
			lua_getglobal(L(), fname.c_str());
			if (lua_isnil(L(), -1))
			{
				lua_pop(L(), 2);
				return "";
			}
			
			if (entity)
				Lua::pushEntity(L(), entity);
			else
				lua_pushnil(L());
			Lua::pushStringVector(L(), cmd.arguments);
			lua_pushnumber(L(), _engine->get_client(cmd.client)->userid());

			int result = lua_pcall(L(), 3, 2, 1);
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
		
		int System::process_action(Action* action, const std::string& fname)
		{
			lua_getglobal(L(), "_backtracer");
			
			lua_getglobal(L(), fname.c_str());
			if (lua_isnil(L(), -1))
			{
				lua_pop(L(), 2);
				return -1;
			}
			
			Lua::pushAction(L(), action);
			
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
		
		void System::startup(const std::string& fname)
		{
			lua_getglobal(L(), "_backtracer");
			
			lua_getglobal(L(), fname.c_str());
			if (lua_isnil(L(), -1))
			{
				lua_pop(L(), 2);
				return;
			}
			
			int result = lua_pcall(L(), 0, 0, 1);
			if (result)
			{
				if (result == LUA_ERRRUN)
				{
					std::cerr << "Lua runtime error: " << std::endl;
					std::cerr << lua_tostring(L(), -1);
					lua_pop(L(), lua_gettop(L()));
					return;
				}
				else if (result == LUA_ERRERR)
				{
					std::cerr << "Lua error handler error" << std::endl;
					return;
				}
			}
			
			lua_pop(L(), 1); //pop the backtracer
		}
		
		void System::initFunctions()
		{
			Lua::pushEngine(L(), _engine);
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
	}
}