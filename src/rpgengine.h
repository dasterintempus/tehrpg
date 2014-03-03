#pragma once	

#include <string>
#include <map>
#include <queue>
#include <SFML/System.hpp>

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
		
		class Engine
		{
			public:
				
				static const long int WorldXSize = 2048;
				static const long int WorldYSize = 2048;
				
				Engine(Application* parent, GameServer* server);
				~Engine();
			
				void init();
				void start();
				void finish();
			
				Entity* get_entity(unsigned int id);
				System* get_system(const std::string& name);
			
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
				
				void queue_action(unsigned int entityid, Action* action);
				
				void logout(const clientid& client);
			
				GameClient* get_client(const clientid& client);
				
				static bool valid_coord(const long int& xpos, const long int& ypos);
			private:
				void tick();
				void cleanup();
			
				Application* _parent;
				GameServer* _server;
				std::map<unsigned int, Entity*> _entities;
				std::map<std::string, System*> _systems;
				std::map<clientid, unsigned int> _client2pc;
				std::map<unsigned int, std::queue<Action*> > _entityactions;
				
				bool _done;
				sf::Mutex _donemutex;
		};
	}
}