#pragma once

#include <string>
#include <map>
#include <vector>

namespace teh
{
namespace RPG
{
	class Game;
	class Tile;
	class Character;
	class ItemInstance;
	
	class Inventory
	{
		public:
			static Inventory* build(Game* parent, Tile* tile);	
			static Inventory* build(Game* parent, Character* character, std::string name, unsigned short int capacity);
			
			Inventory(unsigned int id, Game* parent);
		
			unsigned int id();
			std::string name();
			unsigned short int capacity();
		
			Character* character();
			Tile* tile();
		
			std::vector<ItemInstance*> contents();
		
			unsigned short int space_used();
			unsigned short int space_remaining();
		
			ItemInstance* select(const std::string& target, unsigned int targetn=1);
		
			bool acquire(ItemInstance* item);
			std::string describe_contents();
		private:
			unsigned int _id;
			Game* _parent;
			std::string _name;
			unsigned short int _capacity;
	};
}
}