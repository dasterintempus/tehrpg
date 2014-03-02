#pragma once

#include "commandparser.h"

namespace teh
{
	class GameServer;
	
	class MetagameCommandHandler
		: public CommandHandlerInterface
	{
		public:
			MetagameCommandHandler(GameServer* parent);
			
			void handle_command(const Command& cmd);
			bool accepts_command(const Command& cmd);
		private:
			GameServer* _parent;
	};
	
}