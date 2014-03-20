#include "db.h"

namespace teh
{
	Database::Database(const std::string& pipepath)
		: _pipepath(pipepath), _done(true)
	{
		
	}
	
	Database::~Database()
	{
		_pipe.close();
	}
	
	void Database::init()
	{
		_pipe.open(_pipepath);
	}
	
	void Database::start()
	{
		init();
		
		_donemutex.lock();
		while (!_done)
		{
			_donemutex.unlock();
			
			sf::sleep(sf::milliseconds(1));
			
			_donemutex.lock();
		}
		_donemutex.unlock();
	}
	
	void Database::finish()
	{
		sf::Lock donelock(_donemutex);
		_done = true;
	}

	Json::Value Database::get(const std::string& path)
	{
		
	}
	
	void Database::set(const std::string& path, const Json::Value& value)
	{
		
	}
	
	std::vector<std::string> Database::list(const std::string& path)
	{
		
	}
}