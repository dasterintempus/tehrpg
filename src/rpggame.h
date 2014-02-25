#pragma once	

#include <string>
#include <map>

#include "typedefs.h"

namespace teh
{	
	class Application;
	class GameServer;
	class GameClient;
	class MySQL;
namespace RPG
{
	class Character;
	class Tile;
	class Inventory;
	class ItemType;
	class ItemInstance;
	class Object;
	
	class Game
	{
		public:
			
			static const long int WorldXSize = 65535;
			static const long int WorldYSize = 65535;
			
			Game(Application* parent, GameServer* server);
			~Game();
		
			void init();
			void start();
			void finish();
		
			clientid check_logged_in(const std::string& charactername);
			clientid check_logged_in(Character* character);
		
			void logout(const clientid& client);
		
			Character* select_character(const clientid& client, const std::string& charactername);
			Character* get_character(unsigned int id);
			Character* get_active_character(const clientid& client);
			stringvector character_names(const clientid& client);
		
			Tile* get_tile(unsigned int id);
			void locate_tile(const long int& xpos, const long int& ypos, Tile* tile);
			Tile* find_tile(const long int& xpos, const long int& ypos);
			
			Inventory* get_inventory(unsigned int id);
			
			ItemType* get_itemtype(unsigned int id);
			ItemType* find_itemtype(const std::string& name);
			
			ItemInstance* get_iteminstance(unsigned int id);
			
			Object* get_object(unsigned int objectid);
		
			MySQL* sql();
			void message_client(const clientid& client, const std::string& message);
		
			GameClient* get_client(const clientid& client);
			
			static bool valid_coord(const long int& xpos, const long int& ypos);
		private:
			Application* _parent;
			GameServer* _server;
			std::map<clientid, Character*> _activecharacters;
			std::map<unsigned int, Character*> _characters;
			std::map<long int, std::map<long int, Tile*> > _tilescoords;
			std::map<unsigned int, Tile*> _tiles;
			std::map<unsigned int, Inventory*> _inventories;
			std::map<unsigned int, ItemType*> _itemtypes;
			std::map<unsigned int, ItemInstance*> _iteminstances;
			std::map<unsigned int, Object*> _objects;
	};
}
}