#pragma once

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
		
		const unsigned int ProxyComponent = 1;
		const unsigned int ProxyEntity = 2;
		const unsigned int ProxyEngine = 3;
		const unsigned int ProxySystem = 4;
		
		struct LuaProxy
		{
			union
			{
				Component* component;
				Entity* entity;
				Engine* engine;
				System* system;
			};
			unsigned int type;
		};
		
		/*
		struct ComponentProxy
		{
			Component* component;
			//Engine* engine;
		};
		
		struct EntityProxy
		{
			Entity* entity;
			//Engine* engine;
		};
		
		struct EngineProxy
		{
			Engine* engine;
		};
		
		struct SystemProxy
		{
			System* system;
		};
		*/
		
		class System
		{
			public:
				System(const std::string& name, Engine* engine);
				~System();
			
				std::string process(Entity* entity);
				void initFunctions();
			
				lua_State* L();
			
				static void pushComponent(lua_State* L, Component* component);
				static void pushEntity(lua_State* L, Entity* entity);
				static void pushEngine(lua_State* L, Engine* engine);
				static void pushSystem(lua_State* L, System* system);
			
				static int copyStack(lua_State* sourceL, int startindex, lua_State* destL);
			private:
				std::string _name;
				Engine* _engine;
				lua_State* _L;
		};
	}
}