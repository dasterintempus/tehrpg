#pragma once

#include <json/json.h>
#include <string>
#include <vector>
#include <fstream>
#include <SFML/System.hpp>

namespace teh
{
	class Database
	{
		public:
			Database(const std::string& pipepath);
			~Database();
			
			void init();
			void start();
			void finish();
		
			Json::Value get(const std::string& path);
			void set(const std::string& path, const Json::Value& value);
			std::vector<std::string> list(const std::string& path);
		private:
			std::string _pipepath;
			std::fstream _pipe;
			sf::Mutex _reqmutex;
			sf::Mutex _donemutex;
			bool _done;
	};
}