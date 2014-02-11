#include "consoleconnection.h"
#include <iostream>

namespace teh
{	
	ConsoleConnection::ConsoleConnection()
		: _done(false)
	{
		
	}
	
	void ConsoleConnection::start()
	{
		_donemutex.lock();
		while (!_done)
		{
			_donemutex.unlock();
			
			std::string line;
		
			std::getline(std::cin, line);
			
			_inbuffermutex.lock();
			_inbuffer << line << std::endl;
			_inbuffermutex.unlock();
			
			_donemutex.lock();
		}
		_donemutex.unlock();
	}
	
	void ConsoleConnection::finish()
	{
		sf::Lock donelock(_donemutex);
		_done = true;
	}
	
	bool ConsoleConnection::has_line()
	{
		sf::Lock inbufferlock(_inbuffermutex);
		return _inbuffer.str().find('\n') != std::string::npos;
	}
	
	std::string ConsoleConnection::read_line()
	{
		sf::Lock inbufferlock(_inbuffermutex);
		std::string inbuffercopy = _inbuffer.str();
		size_t pos = inbuffercopy.find('\n');
		if (pos == std::string::npos)
		{
			return std::string();
		}
		else
		{
			std::string line = inbuffercopy.substr(0, pos);
			std::cerr << "line: " << line << std::endl;
			inbuffercopy = inbuffercopy.substr(pos+1);
			//std::cerr << "inbuffercopy: " << inbuffercopy << std::endl;
			_inbuffer.str(inbuffercopy);
			return line;
		}
	}
	
	void ConsoleConnection::write_line(const std::string& line)
	{
		std::cout << line << std::endl;
	}

	void ConsoleConnection::close()
	{
		//pass	
	}
	
}