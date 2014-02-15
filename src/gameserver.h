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
			
			const static unsigned short int RootPermissions = 65535;
			const static unsigned short int UserPermissions = 1;
			const static unsigned short int ServerAdminPermissions = 2;
			const static unsigned short int ContentAdminPermissions = 4;
						
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
		
			GameClient* get_client(const clientid& id);
		
			MySQL* sql();
		
			bool kill(const std::string& username, const std::string& killer);
			bool kill(const clientid& id, const std::string& killer);
		
			void shutdown(const std::string username);
		private:
			//methods
			void process_line(const clientid& id, const std::string& line);
			
			//bool try_register(GameClient* client, const std::string& password);
		
			void update_permissions(GameClient* client);
		
			//std::string process_root_command(GameClient* client, const Command& cmd);
			//std::string process_admin_command(GameClient* client, const Command& cmd);
			//std::string process_user_command(GameClient* client, const Command& cmd);
		
			std::string greeting(const clientid& id);
		
			clientid find_clientid(GameClient* client);
			clientid find_from_username(const std::string& username);
		
			//members
			std::map<clientid, GameClient*> _clients;
		
			clientid _next;
			bool _done;
			sf::Mutex _donemutex;
			sf::Mutex _clientsmutex;
		
			Application* _parent;
	};
}