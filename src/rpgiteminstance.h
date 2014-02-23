#pragma once

#include <string>

namespace teh
{
	class RPGGame;
	class RPGInventory;
	class RPGItemType;
	
	class RPGItemInstance
	{
		public:
			static RPGItemInstance* build(RPGGame* parent, RPGInventory* inv, RPGItemType* type);
			RPGItemInstance(unsigned int id, RPGGame* parent);
		
			unsigned int id();
			RPGInventory* container();
			RPGItemType* type();
		private:
			unsigned int _id;
			RPGGame* _parent;
	};
}