#pragma once	

#include <string>
#include <map>
#include <queue>
#include <utility>
#include <SFML/System.hpp>
#include "jsonutil.h"

#include "typedefs.h"

namespace teh
{	
	class Application;
	class GameServer;
	class GameClient;
	class MySQL;
	namespace RPG
	{
		class Entity;
		class Component;
		class System;
		class Action;
		class World;
		
		class Engine
		{
			public:
				
				static const long int WorldXSize = 2048;
				static const long int WorldYSize = 2048;
				
				Engine(Application* parent, const std::string& module, GameServer* server);
				~Engine();
			
				void init();
				void start();
				void finish();
			
				std::string get_etc(const std::string& key);
				void set_etc(const std::string& key, const std::string& value);
			
				Entity* get_entity(unsigned int id);
				bool activate_entity(Entity* entity);
				void delete_entity(Entity* entity);
			
				World* get_world();
			
				System* get_system(const std::string& name);
			
				std::string get_module_path() const;
				const Json::Value& get_module_config() const;
			
				void	firstinit();
			
				std::pair<long int, long int> globalspawnpoint();
			
				MySQL* sql();
				void message_client(const clientid& client, const std::string& message);
			
				Entity* get_pc(const clientid& client);
				clientid get_player(Entity* entity);
				bool is_pc_active(unsigned int entityid);
				bool is_pc_name_active(const std::string& name);
				bool set_pc_played_by(unsigned int entityid, const clientid& client);
				bool set_pc_name_played_by(const std::string& name, const clientid& client);
				std::vector<std::string> get_character_names_of_user(unsigned int userid);
				std::vector<std::string> get_character_names_of_client(const clientid& client);
				
				void message_entity(Entity* entity, const std::string& msg);
				Entity* findCharacter(const std::string& name);
				
				void queue_action(unsigned int entityid, Action* action);
				
				void logout(const clientid& client);
			
				GameClient* get_client(const clientid& client);
				
				static bool valid_coord(const long int& xpos, const long int& ypos);
			private:
				void tick();
				void cleanup();
			
				Application* _parent;
				GameServer* _server;
				std::string _module;
				Json::Value _moduleconfig;
				std::map<unsigned int, Entity*> _entities;
				std::map<std::string, System*> _systems;
				std::map<clientid, unsigned int> _client2pc;
				std::map<unsigned int, std::queue<Action*> > _entityactions;
				std::map<std::string, std::string> _etc;
				World* _world;
				bool _done;
				sf::Mutex _donemutex;
				std::string _actionhandler;
		};
	}
}