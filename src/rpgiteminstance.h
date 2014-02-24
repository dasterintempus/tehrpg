#pragma once

#include <string>

namespace teh
{
namespace RPG
{
	class Game;
	class Inventory;
	class ItemType;
	
	class ItemInstance
	{
		public:
			static ItemInstance* build(Game* parent, Inventory* inv, ItemType* type);
			ItemInstance(unsigned int id, Game* parent);
		
			unsigned int id();
			Inventory* container();
			ItemType* type();
		private:
			unsigned int _id;
			Game* _parent;
	};
}
}