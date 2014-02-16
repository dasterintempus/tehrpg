#pragma once	

#include <iostream>
#include <string>
#include <sstream>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <map>

#include "gameserver_interface.h"
#include "typedefs.h"

namespace teh
{
	class GameServer;
	
	class ReuseTcpListener
		: public sf::TcpListener
	{
		public:
			void set_reuse(bool opt = true);
	};
	
	
	typedef std::pair<sf::IpAddress, unsigned short int> ipendpoint;
	
	ipendpoint get_remote_endpoint(const sf::TcpSocket& socket);
	
	class NetServer;
	class NetConnection;
	
	struct NetConnectionThreadStartReceiveFunctor
	{
		NetConnection* netconnptr;
		void operator()();
	};
	
	struct NetConnectionThreadStartSendFunctor
	{
		NetConnection* netconnptr;
		void operator()();
	};
	
	class NetConnection
		: public GameConnectionInterface
	{
		public:
			NetConnection(sf::TcpSocket* socket, NetServer* parent);
			~NetConnection();
		
			bool is_done();
		
			bool has_line();
			std::string read_line();
			
			void write_line(const std::string& line);
		
			void close();
		
			void start_receive();
			void start_send();
		private:
			sf::TcpSocket* _socket;
			NetServer* _parent;
		
			bool _done;
			sf::Mutex _donemutex;
		
			std::stringstream _inbuffer;
			std::stringstream _outbuffer;
		
			sf::Mutex _outbuffermutex;
			sf::Mutex _inbuffermutex;
	};
	
	class NetServer
	{
		public:
			NetServer(const unsigned short int& port, GameServerInterface* gameserv);
			~NetServer();
		
			void start();
			bool is_done() const;
			void finish();
		private:
			void check_done_clients();
			void add_client(sf::TcpSocket* socket);
		
			GameServerInterface* _gameserv;
		
			bool _done;
			sf::Mutex _donemutex;
			unsigned short int _port;
			ReuseTcpListener _listener;
			std::map<ipendpoint, NetConnection*> _connections;
			std::map<ipendpoint, sf::Thread*> _receive_threads;
			std::map<ipendpoint, sf::Thread*> _send_threads;
			std::map<ipendpoint, clientid> _clientids;
	};
}