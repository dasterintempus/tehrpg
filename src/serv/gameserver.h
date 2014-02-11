#pragma once

#include <SFML/System.hpp>
#include <map>

#include "gameserver_interface.h"
#include "typedefs.h"

namespace teh
{
	class Application;
	
	class GameClient
		: public GameConnectionInterface
	{
		public:
			enum State
			{
				WelcomeState,
				UsernameState,
				PasswordState,
				LoggedInState,
				RegisterUsernameState,
				RegisterPasswordState,
				ClosingState,
			};
			
			enum Permissions
			{
				NoPermissions = 0,
				UserPermissions = 1,
				AdminPermissions = 2,
				RootPermissions = 4,
			};
			
			GameClient(GameConnectionInterface* conn);
			
			bool has_line();
			std::string read_line();
			
			void write_line(const std::string& line);
		
			void close();
			
			GameClient::State state();
			void state(const GameClient::State& s);
			
			std::string username();
			void username(const std::string& un);
			
			GameClient::Permissions permissions();
			void permissions(const GameClient::Permissions& p);
		private:
			std::string _username;
			GameClient::State _state;
			GameClient::Permissions _permissions;
			GameConnectionInterface* _conn;
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
		
			void start();
			void finish();
		private:
			//methods
			void process_line(const clientid& id, const std::string& line);
		
			bool try_login(GameClient* client, const std::string& password);
			bool try_register(GameClient* client, const std::string& password);
		
			void update_permissions(GameClient* client);
		
			std::string process_root_command(GameClient* client, const std::string& line);
			std::string process_admin_command(GameClient* client, const std::string& line);
			std::string process_user_command(GameClient* client, const std::string& line);
		
			std::string greeting(const clientid& id);
		
			clientid find_clientid(GameClient* client);
		
			//members
			std::map<clientid, GameClient*> _clients;
		
			clientid _next;
			bool _done;
			sf::Mutex _donemutex;
			sf::Mutex _clientsmutex;
		
			Application* _parent;
	};
}