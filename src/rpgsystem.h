#pragma once

#include "commandparser.h"

#include <string>
#include <lua.hpp>

namespace teh
{
	namespace RPG
	{
		class Engine;
		class Entity;
		class Component;
		class System;
		class Action;
		
		const unsigned int ProxyComponent = 1;
		const unsigned int ProxyEntity = 2;
		const unsigned int ProxyEngine = 3;
		const unsigned int ProxySystem = 4;
		const unsigned int ProxyAction = 5;
		
		struct LuaProxy
		{
			union
			{
				Component* component;
				Entity* entity;
				Engine* engine;
				System* system;
				Action* action;
			};
			unsigned int type;
		};
		
		class System
		{
			public:
				System(const std::string& name, Engine* engine);
				~System();
			
				std::string process_command(Entity* entity, const Command& cmd, bool& ok);
				int process_action(Action* action);
				
				void initFunctions();
			
				lua_State* L();
			
				static void pushComponent(lua_State* L, Component* component);
				static void pushEntity(lua_State* L, Entity* entity);
				static void pushEngine(lua_State* L, Engine* engine);
				static void pushSystem(lua_State* L, System* system);
				static void pushAction(lua_State* L, Action* action);
			
				static void pushStringVector(lua_State* L, const std::vector<std::string>& vec);
			
				static int copyStack(lua_State* sourceL, int startindex, lua_State* destL);
				static bool copyStackValue(lua_State* sourceL, int index, lua_State* destL);
			private:
				std::string _name;
				Engine* _engine;
				lua_State* _L;
		};
	}
}