#pragma once	

#include <string>
#include <map>

#include "typedefs.h"

namespace teh
{	
	class Application;
	class GameServer;
	class RPGCharacter;
	class RPGRoom;
	class MySQL;
	
	class RPGGame
	{
		public:
			RPGGame(Application* parent, GameServer* server);
			~RPGGame();
		
			void init();
			void start();
			void finish();
		
			clientid check_logged_in(RPGCharacter* character);
		
			RPGCharacter* select_character(const clientid& client, const std::string& username, const std::string& charactername);
			RPGCharacter* get_character(int id);
			RPGCharacter* get_active_character(const clientid& client);
		
			RPGRoom* get_room(int id);
			void locate_room(long int x, long int y, short int z, RPGRoom* room);
		
			MySQL* sql();
			void message_client(const clientid& client, const std::string& message);
		private:
			Application* _parent;
			GameServer* _server;
			std::map<clientid, RPGCharacter*> _activecharacters;
			std::map<int, RPGCharacter*> _characters;
			std::map<long int, std::map<long int, std::map<short int, RPGRoom*> > > _roomscoords;
			std::map<int, RPGRoom*> _rooms;
	};
}