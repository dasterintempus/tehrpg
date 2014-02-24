#pragma once

#include <string>
#include <map>
#include <boost/regex.hpp>

namespace teh
{	
	class RPGGame;
	class RPGTile;
	class RPGInventory;
	
	class RPGCharacter
	{
		public:
			const static std::string StatNames[6];
			static RPGCharacter* build(RPGGame* parent, const std::string& name, const std::string& username, RPGTile* room, const std::map<std::string, unsigned short int>& stats);
		
			RPGCharacter(unsigned int id, RPGGame* parent);
		
			unsigned short int strength();
			unsigned short int constitution();
			unsigned short int dexterity();
			unsigned short int intelligence();
			unsigned short int wisdom();
			unsigned short int charisma();
		
			std::string name();
		
			RPGTile* get_location();
		
			void say(const std::string& msg);
			void emote(const std::string& msg, bool possessive=false);
			RPGTile* move(const std::string& direction);
			std::string look();
			std::string pickup(const std::string& target, unsigned int targetn=0, const std::string& destination="backpack");
			std::string drop(const std::string& target, unsigned int targetn=0, const std::string& origin="backpack");
		
			RPGInventory* get_inventory(const std::string& name);
			RPGInventory* add_inventory(const std::string& name, unsigned short int capacity);
			std::vector<RPGInventory*> all_inventories();
		
			unsigned int id();
		private:
			void update_location(RPGTile* destination);
		
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