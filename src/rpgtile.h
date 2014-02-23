#pragma once

#include <string>
#include <map>

#include "typedefs.h"

namespace teh
{	
	class RPGGame;
	class RPGCharacter;
	
	class RPGTile
	{
		public:
			const static std::string northdir;
			const static std::string southdir;
			const static std::string eastdir;
			const static std::string westdir;
		
			static RPGTile* build(RPGGame* parent, const long int& xpos, const long int& ypos, bool solid = true, const std::string& description = "");
			RPGTile(unsigned int id, RPGGame* parent);
		
			std::vector<RPGCharacter*> get_occupants(bool loggedin=true);
			void broadcast(const std::string& msg);
		
			unsigned int id();
		
			long int xpos();
			long int ypos();
		
			bool solid();
		
			std::string description();
		
			bool can_exit_north();
			bool can_exit_south();
			bool can_exit_east();
			bool can_exit_west();
		
			stringvector get_wall_sides();
			stringvector get_exits();
		
			RPGTile* can_exit(const int& dx, const int& dy);
			RPGTile* can_exit(const std::string& direction);
		private:
			void locate();
		
			unsigned int _id;
			RPGGame* _parent;
			std::string _description;
			long int _xpos;
			long int _ypos;
			bool _solid;
	};
	
}
	