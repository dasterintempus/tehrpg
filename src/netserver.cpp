#include "netserver.h"
#include <sys/socket.h>

namespace teh
{
	//
	// ReuseTcpListener
	//
	void ReuseTcpListener::set_reuse(bool opt)
	{
		//set SO_REUSEADDR
		int handle = getHandle();
		int optval;
		if (opt)
			optval = 1;
		else
			optval = 0;
		setsockopt(handle, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
	}
	
	//
	// NetConnection
	//
	
	void NetConnectionThreadStartReceiveFunctor::operator()()
	{
		netconnptr->start_receive();
	}
	
	void NetConnectionThreadStartSendFunctor::operator()()
	{
		netconnptr->start_send();
	}
	
	ipendpoint get_remote_endpoint(const sf::TcpSocket& socket)
	{
		return std::make_pair(socket.getRemoteAddress(), socket.getRemotePort());
	}
	
	NetConnection::NetConnection(sf::TcpSocket* socket, NetServer* parent)
		: _socket(socket), _parent(parent), _done(false)
	{
		_socket->setBlocking(false);
	}
	
	NetConnection::~NetConnection()
	{
		delete _socket;
	}
	
	bool NetConnection::is_done()
	{
		sf::Lock donelock(_donemutex);
		return _done;
	}
	
	bool NetConnection::has_line()
	{
		sf::Lock inbufferlock(_inbuffermutex);
		return _inbuffer.str().find("\n") != std::string::npos;
	}
	
	std::string NetConnection::read_line()
	{
		sf::Lock inbufferlock(_inbuffermutex);
		std::string inbuffercopy = _inbuffer.str();
		size_t pos = inbuffercopy.find("\n");
		if (pos == std::string::npos)
		{
			return std::string();
		}
		else
		{
			std::string line = inbuffercopy.substr(0, pos);
			std::cerr << "line: " << line << std::endl;
			if (inbuffercopy.size() > pos)
			{
				inbuffercopy = inbuffercopy.substr(pos+1);
				_inbuffer.str(inbuffercopy);
			}
			else
			{
				_inbuffer.str("");
			}
			return line;
		}
	}
	
	void NetConnection::write_line(const std::string& line)
	{
		sf::Lock outbufferlock(_outbuffermutex);
		_outbuffer << line << "\n";
	}
	
	void NetConnection::close()
	{
		sf::Lock donelock(_donemutex);
		_done = true;
		_socket->disconnect();
	}
	
	void NetConnection::start_receive()
	{
		_donemutex.lock();
		while (!_done)
		{
			_donemutex.unlock();
			char buffer[1024];
			std::size_t received = 0;
			sf::Socket::Status receivestatus = _socket->receive(buffer, sizeof(buffer), received);
			std::string instr(buffer, received);
			switch (receivestatus)
			{
				case sf::Socket::Done:
					_inbuffermutex.lock();
					_inbuffer << instr;
					_inbuffermutex.unlock();
				
					std::cerr << "got data in: " << instr;
					
					//Debug
					//_outbuffermutex.lock();
					//_outbuffer << instr;
					//_outbuffermutex.unlock();
				
					break;
				case sf::Socket::NotReady:
					break;
				default:
					std::cerr << "Receive error or disconnect" << std::endl;
					_donemutex.lock();
					_done = true;
					_donemutex.unlock();
			}
			_donemutex.lock();
			if (_parent->is_done())
				_done = true;
		}
		_donemutex.unlock();
	}
	
	void NetConnection::start_send()
	{
		_donemutex.lock();
		while (!_done)
		{
			_donemutex.unlock();
			if (_outbuffer.str() != "")
			{
				std::string str_out;
				
				_outbuffermutex.lock();
				str_out = _outbuffer.str();
				_outbuffer.str("");
				_outbuffermutex.unlock();
				
				std::cerr << "Trying to send out: " << str_out;
				sf::Socket::Status sendstatus = _socket->send(str_out.c_str(), str_out.size());
				switch (sendstatus)
				{
					case sf::Socket::Done:
						break;
					case sf::Socket::NotReady:
						std::cerr << "Socket not ready to send" << std::endl;
						_outbuffer << str_out;
						break;
					default:
						std::cerr << "Send error or disconnect" << std::endl;
						_donemutex.lock();
						_done = true;
						_donemutex.unlock();
				}
			}
			_donemutex.lock();
			if (_parent->is_done())
				_done = true;
		}
		_donemutex.unlock();
	}
	
	//
	// NetServer
	//
	
	NetServer::NetServer(const unsigned short int& port, GameServerInterface* gameserv)
		: _port(port), _gameserv(gameserv), _done(false)
	{
		_listener.set_reuse();
		//Set not blocking too
		_listener.setBlocking(false);
	}
	
	NetServer::~NetServer()
	{
		_done = true;
		for (std::map<ipendpoint, sf::Thread*>::iterator i = _receive_threads.begin(); i != _receive_threads.end(); i++)
		{
			delete (*i).second;
		}
		_receive_threads.clear();
		for (std::map<ipendpoint, sf::Thread*>::iterator i = _send_threads.begin(); i != _send_threads.end(); i++)
		{
			delete (*i).second;
		}
		_send_threads.clear();
		for (std::map<ipendpoint, NetConnection*>::iterator i = _connections.begin();i != _connections.end(); i++)
		{
			delete (*i).second;
		}
		_connections.clear();
	}
	
	void NetServer::start()
	{
		sf::Socket::Status listenstatus;
		do
		{
			_listener.set_reuse();
			listenstatus = _listener.listen(_port);
			sf::sleep(sf::seconds(1));
		} while (listenstatus != sf::Socket::Done);
		
		_donemutex.lock();
		while (!_done)
		{
			_donemutex.unlock();
			sf::TcpSocket* client = new sf::TcpSocket();
			sf::Socket::Status result = _listener.accept(*client);
			if (result == sf::Socket::Done)
			{
				std::cerr << "Got connection from " << client->getRemoteAddress() << std::endl;
				add_client(client);
			}
			else
			{
				delete client;
			}
			
			check_done_clients();
			
			sf::sleep(sf::milliseconds(1));
			
			_donemutex.lock();
		}
		_donemutex.unlock();
		
		std::cerr << "Netserver done, returning from start thread" << std::endl;
	}
	
	bool NetServer::is_done() const
	{
		return _done;
	}
	
	void NetServer::finish()
	{
		sf::Lock donelock(_donemutex);
		std::cerr << "Acquired netserver donelock" << std::endl;
		_done = true;
	}
	
	void NetServer::check_done_clients()
	{
		std::vector<ipendpoint> dead;
		for (std::map<ipendpoint, NetConnection*>::iterator i = _connections.begin(); i != _connections.end(); i++)
		{
			if ((*i).second->is_done())
			{
				_receive_threads.erase((*i).first);
				_send_threads.erase((*i).first);
				_gameserv->remove_client(_clientids[(*i).first]);
				delete (*i).second;
				dead.push_back((*i).first);
			}
		}
		for (unsigned int n = 0; n < dead.size(); n++)
		{
			_connections.erase(dead[n]);
		}
	}
	
	void NetServer::add_client(sf::TcpSocket* socket)
	{
		ipendpoint remote = get_remote_endpoint(*socket);
		
		NetConnection* netconn = new NetConnection(socket, this);
		_connections[remote] = netconn;
		
		NetConnectionThreadStartReceiveFunctor rfunctor;
		rfunctor.netconnptr = netconn;
		
		NetConnectionThreadStartSendFunctor sfunctor;
		sfunctor.netconnptr = netconn;
		
		sf::Thread* netconnrthread = new sf::Thread(rfunctor);
		sf::Thread* netconnsthread = new sf::Thread(sfunctor);
		
		_receive_threads[remote] = netconnrthread;
		_send_threads[remote] = netconnsthread;
		
		netconnrthread->launch();
		netconnsthread->launch();
		
		_clientids[remote] = _gameserv->add_connection(netconn);
	}
}