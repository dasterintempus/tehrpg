#pragma once

#include <string>
#include <map>

namespace teh
{
	class RPGGame;
	class RPGTile;
	class RPGCharacter;
	class RPGInventorySlot;
	class RPGInventory;
	
	class RPGItemType
	{
		public:
			static RPGItemType* build(RPGGame* parent, const std::string& name, const std::string& description, unsigned short int size, bool stackable);
			
			RPGItemType(unsigned int id, RPGGame* parent);
		
			unsigned int id();
			std::string name();
			std::string description();
			unsigned short int size();
		private:
			unsigned int _id;
			RPGGame* _parent;
		
			std::string _name;
			std::string _description;
			unsigned short int _size;
	};
}