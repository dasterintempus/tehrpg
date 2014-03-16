#pragma once

#include "commandparser.h"

#include <string>
#include <lua.hpp>

#include <iostream>

namespace teh
{
	namespace RPG
	{
		class Engine;
		class Entity;
		class Component;
		class System;
		class Action;
		class World;
		
		namespace Lua
		{
			const unsigned int ProxyComponent = 1;
			const unsigned int ProxyEntity = 2;
			const unsigned int ProxyEngine = 3;
			const unsigned int ProxySystem = 4;
			const unsigned int ProxyAction = 5;
			const unsigned int ProxyWorld = 6;
			
			struct Proxy
			{
				union
				{
					Component* component;
					Entity* entity;
					Engine* engine;
					System* system;
					Action* action;
					World* world;
				};
				unsigned int type;
			};
			
			//luautil.cpp
			int Util_noise2d(lua_State* L);
			int Util_octave_noise2d(lua_State* L);
			int Util_noise3d(lua_State* L);
			int Util_octave_noise3d(lua_State* L);
			int Util_image(lua_State* L);
			void pushStringVector(lua_State* L, const std::vector<std::string>& vec);
			int copyStack(lua_State* sourceL, int startindex, lua_State* destL);
			bool copyStackValue(lua_State* sourceL, int index, lua_State* destL);
			
			//luacomponent.cpp
			int CP_mm_index(lua_State* L);
			int CP_mm_newindex(lua_State* L);
			void pushComponent(lua_State* L, Component* component);
			
			//luaentity.cpp
			int EP_component_mm_index(lua_State* L);
			int EP_act_fcall(lua_State* L);
			int EP_msg_fcall(lua_State* L);
			int EP_attach_fcall(lua_State* L);
			int EP_detach_fcall(lua_State* L);
			int EP_destroy_fcall(lua_State* L);
			int EP_setuserid_fcall(lua_State* L);
			int EP_mm_index(lua_State* L);
			int EP_mm_eq(lua_State* L);
			void pushEntity(lua_State* L, Entity* entity);
			
			//luaengine.cpp
			int NP_system_mm_index(lua_State* L);
			int NP_new_entity_fcall(lua_State* L);
			int NP_etc_fcall(lua_State* L);
			int NP_select_fcall(lua_State* L);
			int NP_playchar_fcall(lua_State* L);
			int NP_utility_mm_index(lua_State* L);
			int NP_mm_index(lua_State* L);
			void pushEngine(lua_State* L, Engine* engine);
			
			//luasystem.cpp
			int SP_subsystem_mm_call(lua_State* L);
			int SP_mm_index(lua_State* L);
			void pushSystem(lua_State* L, System* system);
			
			//luaaction.cpp
			int AP_mm_index(lua_State* L);
			void pushAction(lua_State* L, Action* action);
			
			//luaworld.cpp
			int WP_save_fcall(lua_State* L);
			int WP_load_fcall(lua_State* L);
			int WP_init_size_fcall(lua_State* L);
			int WP_size_fcall(lua_State* L);
			int WP_set_map_value_fcall(lua_State* L);
			int WP_get_map_value_fcall(lua_State* L);
			int WP_set_map_flag_fcall(lua_State* L);
			int WP_get_map_flag_fcall(lua_State* L);
			int WP_mm_index(lua_State* L);
			void pushWorld(lua_State* L, World* world);
		}
		
		
		class System
		{
			public:
				System(const std::string& name, Engine* engine);
				~System();
			
				std::string process_command(Entity* entity, const Command& cmd, const std::string& fname, bool& ok);
				int process_action(Action* action, const std::string& fname);
				void startup(const std::string& fname);
				
				void initFunctions();
			
				lua_State* L();
			private:
				std::string _name;
				Engine* _engine;
				lua_State* _L;
		};
	}
}