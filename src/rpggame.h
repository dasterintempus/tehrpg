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
	class RPGRoom;
	class MySQL;
	
	class RPGGame
	{
		public:
			
			static const long int WorldXSize = 65535;
			static const long int WorldYSize = 65535;
			static const short int WorldZSize = 2;
			
			RPGGame(Application* parent, GameServer* server);
			~RPGGame();
		
			void init();
			void start();
			void finish();
		
			clientid check_logged_in(const std::string& charactername);
			clientid check_logged_in(RPGCharacter* character);
		
			RPGCharacter* select_character(const clientid& client, const std::string& charactername);
			RPGCharacter* get_character(int id);
			RPGCharacter* get_active_character(const clientid& client);
			stringvector character_names(const clientid& client);
		
			RPGRoom* get_room(int id);
			void locate_room(const long int& xpos, const long int& ypos, const short int& zpos, RPGRoom* room);
			RPGRoom* find_room(const long int& xpos, const long int& ypos, const short int& zpos);
		
			MySQL* sql();
			void message_client(const clientid& client, const std::string& message);
		
			GameClient* get_client(const clientid& client);
			
			static bool valid_coord(const long int& xpos, const long int& ypos, const short int& zpos);
		private:
			Application* _parent;
			GameServer* _server;
			std::map<clientid, RPGCharacter*> _activecharacters;
			std::map<int, RPGCharacter*> _characters;
			std::map<long int, std::map<long int, std::map<short int, RPGRoom*> > > _roomscoords;
			std::map<int, RPGRoom*> _rooms;
	};
}