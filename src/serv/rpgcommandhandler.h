#pragma once

#include "commandparser.h"

namespace teh
{
	class RPGGame;
	
	class RPGCommandHandler
		: public CommandHandlerInterface
	{
		public:
			RPGCommandHandler(RPGGame* parent);
			
			void handle_command(const Command& cmd);
			bool accepts_command(const Command& cmd);
		private:
			RPGGame* _parent;
	};
	
}