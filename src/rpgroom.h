#pragma once

#include <string>
#include <map>

#include "typedefs.h"

namespace teh
{	
	class RPGGame;
	class RPGCharacter;
	
	class RPGRoom
	{
		public:
			RPGRoom(int id, RPGGame* parent);
		
			std::vector<RPGCharacter*> get_occupants();
			void broadcast(const std::string& msg);
		private:
			void locate();
		
			int _id;
			RPGGame* _parent;
			std::string _description;
			long int _xpos;
			long int _ypos;
			short int _zpos;
	};
	
}
	