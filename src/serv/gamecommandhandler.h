#pragma once

#include "commandparser.h"

namespace teh
{
	class GameServer;
	
	class GameCommandHandler
		: public CommandHandlerInterface
	{
		public:
			GameCommandHandler(GameServer* parent);
			
			void handle_command(const Command& cmd);
			bool accepts_command(const Command& cmd);
		private:
			GameServer* _parent;
	};
	
}