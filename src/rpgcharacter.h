#pragma once

#include <string>
#include <map>
#include <boost/regex.hpp>

namespace teh
{	
	class RPGGame;
	class RPGRoom;
	
	class RPGCharacter
	{
		public:
			const static std::string StatNames[6];
			static RPGCharacter* build(RPGGame* parent, const std::string& name, const std::string& username, RPGRoom* room, const std::map<std::string, unsigned short int>& stats);	
		
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
			void emote(const std::string& msg, bool possessive=false);
			RPGRoom* move(const unsigned short int& axis, const short int& delta);
			std::string look();
		
			unsigned int id();
		private:
			void update_location(RPGRoom* destination);
		
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