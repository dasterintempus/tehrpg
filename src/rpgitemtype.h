#pragma once

#include <string>
#include <map>

namespace teh
{
namespace RPG
{
	class Game;
	class Tile;
	class Character;
	class InventorySlot;
	class Inventory;
	
	class ItemType
	{
		public:
			static ItemType* build(Game* parent, const std::string& name, const std::string& summary, const std::string& description, unsigned short int size, unsigned short int mass);
			
			ItemType(unsigned int id, Game* parent);
		
			unsigned int id();
			std::string name();
			std::string summary();
			std::string description();
			unsigned short int size();
			unsigned short int mass();
		private:
			unsigned int _id;
			Game* _parent;
		
			std::string _name;
			std::string _summary;
			std::string _description;
			unsigned short int _size;
			unsigned short int _mass;
	};
}
}