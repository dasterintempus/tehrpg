#pragma once

#include <string>

namespace teh
{	
	class RPGGame;
	class RPGRoom;
	
	class RPGCharacter
	{
		public:
			RPGCharacter(int id, RPGGame* parent);
		
			unsigned short int strength();
			unsigned short int constitution();
			unsigned short int dexterity();
			unsigned short int intelligence();
			unsigned short int wisdom();
			unsigned short int charisma();
		
			std::string name();
		
			RPGRoom* get_location();
		
			void say(const std::string& msg);
		private:
			RPGGame* _parent;
			unsigned int _id;
			std::string _name;
		
			unsigned short int _strength;
			unsigned short int _constitution;
			unsigned short int _dexterity;
			unsigned short int _intelligence;
			unsigned short int _wisdom;
			unsigned short int _charisma;
	};
}