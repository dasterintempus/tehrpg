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
			static RPGRoom* build(RPGGame* parent, const long int& xpos, const long int& ypos, const short int& zpos, const std::string& description);
			RPGRoom(unsigned int id, RPGGame* parent);
		
			std::vector<RPGCharacter*> get_occupants();
			void broadcast(const std::string& msg);
		
			unsigned int id();
		
			long int xpos();
			long int ypos();
			short int zpos();
		
			std::string description();
		
			bool can_exit_north();
			bool can_exit_south();
			bool can_exit_east();
			bool can_exit_west();
			bool can_exit_up();
			bool can_exit_down();
		
			stringvector get_wall_sides();
			stringvector get_exits();
		
			bool can_exit(const int& dx, const int& dy, const int& dz);
		private:
			void locate();
		
			unsigned int _id;
			RPGGame* _parent;
			std::string _description;
			long int _xpos;
			long int _ypos;
			short int _zpos;
	};
	
}
	