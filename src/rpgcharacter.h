#pragma once

#include <string>
#include <map>
#include <boost/regex.hpp>

namespace teh
{	
namespace RPG
{
	class Game;
	class Tile;
	class Inventory;
	
	class Character
	{
		public:
			const static std::string StatNames[6];
			static Character* build(Game* parent, const std::string& name, const std::string& username, Tile* room, const std::map<std::string, unsigned short int>& stats);
		
			Character(unsigned int id, Game* parent);
		
			unsigned short int strength();
			unsigned short int constitution();
			unsigned short int dexterity();
			unsigned short int intelligence();
			unsigned short int wisdom();
			unsigned short int charisma();
		
			std::string name();
		
			Tile* get_location();
		
			void say(const std::string& msg);
			void emote(const std::string& msg, bool possessive=false);
			Tile* move(const std::string& direction);
			std::string look();
			std::string pickup(const std::string& target, unsigned int targetn=1, const std::string& destination="backpack");
			std::string drop(const std::string& target, unsigned int targetn=1, const std::string& origin="backpack");
			std::string examine(const std::string& origin, const std::string& target, unsigned int targetn=1);
		
			Inventory* get_inventory(const std::string& name);
			Inventory* add_inventory(const std::string& name, unsigned short int capacity);
			std::vector<Inventory*> all_inventories();
			
			int carrying_mass();
			int max_carrying_mass();
		
			unsigned int id();
		private:
			void update_location(Tile* destination);
		
			Game* _parent;
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
}