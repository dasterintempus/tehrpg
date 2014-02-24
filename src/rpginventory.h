#pragma once

#include <string>
#include <map>
#include <vector>

namespace teh
{
	class RPGGame;
	class RPGTile;
	class RPGCharacter;
	class RPGItemInstance;
	
	class RPGInventory
	{
		public:
			static RPGInventory* build(RPGGame* parent, RPGTile* tile);	
			static RPGInventory* build(RPGGame* parent, RPGCharacter* character, std::string name, unsigned short int capacity);
			
			RPGInventory(unsigned int id, RPGGame* parent);
		
			unsigned int id();
			std::string name();
			unsigned short int capacity();
		
			RPGCharacter* character();
			RPGTile* tile();
		
			std::vector<RPGItemInstance*> contents();
		
			unsigned short int space_used();
			unsigned short int space_remaining();
		
			RPGItemInstance* select(const std::string& target, unsigned int targetn=1);
		
			bool acquire(RPGItemInstance* item);
			std::string describe_contents();
		private:
			unsigned int _id;
			RPGGame* _parent;
			std::string _name;
			unsigned short int _capacity;
	};
}