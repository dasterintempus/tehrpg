#pragma once	

#include <string>
#include <map>

#include "typedefs.h"

namespace teh
{	
	class Application;
	class GameServer;
	class GameClient;
	class RPGCharacter;
	class RPGTile;
	class RPGInventory;
	class RPGItemType;
	class RPGItemInstance;
	class MySQL;
	
	class RPGGame
	{
		public:
			
			static const long int WorldXSize = 65535;
			static const long int WorldYSize = 65535;
			
			RPGGame(Application* parent, GameServer* server);
			~RPGGame();
		
			void init();
			void start();
			void finish();
		
			clientid check_logged_in(const std::string& charactername);
			clientid check_logged_in(RPGCharacter* character);
		
			void logout(const clientid& client);
		
			RPGCharacter* select_character(const clientid& client, const std::string& charactername);
			RPGCharacter* get_character(unsigned int id);
			RPGCharacter* get_active_character(const clientid& client);
			stringvector character_names(const clientid& client);
		
			RPGTile* get_tile(unsigned int id);
			void locate_tile(const long int& xpos, const long int& ypos, RPGTile* tile);
			RPGTile* find_tile(const long int& xpos, const long int& ypos);
			
			RPGInventory* get_inventory(unsigned int id);
			
			RPGItemType* get_itemtype(unsigned int id);
			RPGItemType* find_itemtype(const std::string& name);
			
			RPGItemInstance* get_iteminstance(unsigned int id);
		
			MySQL* sql();
			void message_client(const clientid& client, const std::string& message);
		
			GameClient* get_client(const clientid& client);
			
			static bool valid_coord(const long int& xpos, const long int& ypos);
		private:
			Application* _parent;
			GameServer* _server;
			std::map<clientid, RPGCharacter*> _activecharacters;
			std::map<unsigned int, RPGCharacter*> _characters;
			std::map<long int, std::map<long int, RPGTile*> > _tilescoords;
			std::map<unsigned int, RPGTile*> _tiles;
			std::map<unsigned int, RPGInventory*> _inventories;
			std::map<unsigned int, RPGItemType*> _itemtypes;
			std::map<unsigned int, RPGItemInstance*> _iteminstances;
	};
}