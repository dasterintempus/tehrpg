#pragma once

#include <SFML/System.hpp>
#include <map>

#include "gameserver_interface.h"
#include "typedefs.h"
#include "commandparser.h"

namespace teh
{
	class Application;
	class MySQL;
	
	class GameClient
		: public GameConnectionInterface
	{
		public:
			enum State
			{
				WelcomeState,
				LoginState,
				LoggedInState,
				PlayingState,
				ClosingState,
			};
			
			const static unsigned short int RootPermissions = 0b100000000000000;
			const static unsigned short int UserPermissions = 0b1;
			const static unsigned short int ServerAdminPermissions = 0b10;
			const static unsigned short int ContentAdminPermissions = 0b100;
						
			GameClient(GameConnectionInterface* conn);
			
			void generate_challenge();
			
			bool has_line();
			std::string read_line();
			
			void write_line(const std::string& line);
		
			void close();
			
			GameClient::State state();
			void state(const GameClient::State& s);
			
			std::string username();
			void username(const std::string& un);
			
			unsigned short int permissions();
			void permissions(const unsigned short int& p);
			
			std::string challenge();
		private:
			std::string _username;
			GameClient::State _state;
			unsigned short int _permissions;
			GameConnectionInterface* _conn;
			std::string _challenge;
	};

	class GameServer
		: public GameServerInterface
	{
		public:
			GameServer(Application* parent);
			~GameServer();
			
			clientid add_connection(GameConnectionInterface* conn);
		
			//void remove_client(GameClientInterface* client);
			void remove_client(const clientid& id);
			
			//void close_client(GameClientInterface* client);
			void close_client(const clientid& id);
		
			void init();
		
			void start();
			void finish();
		
			clientid find_clientid(GameClient* client);
			clientid find_from_username(const std::string& username);
			GameClient* get_client(const clientid& id);
		
			MySQL* sql();
		
			bool kill(const std::string& username, const std::string& killer);
			bool kill(const clientid& id, const std::string& killer);
		
			void shutdown(const std::string username);
			
			void update_permissions(GameClient* client);
		private:
			//methods
			void process_line(const clientid& id, const std::string& line);
			std::string greeting(const clientid& id);
			
		
			//members
			std::map<clientid, GameClient*> _clients;
		
			clientid _next;
			bool _done;
			sf::Mutex _donemutex;
			sf::Mutex _clientsmutex;
		
			Application* _parent;
	};
}