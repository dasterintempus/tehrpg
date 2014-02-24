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
			static RPGItemType* build(RPGGame* parent, const std::string& name, const std::string& summary, const std::string& description, unsigned short int size, unsigned short int mass);
			
			RPGItemType(unsigned int id, RPGGame* parent);
		
			unsigned int id();
			std::string name();
			std::string summary();
			std::string description();
			unsigned short int size();
			unsigned short int mass();
		private:
			unsigned int _id;
			RPGGame* _parent;
		
			std::string _name;
			std::string _summary;
			std::string _description;
			unsigned short int _size;
			unsigned short int _mass;
	};
}