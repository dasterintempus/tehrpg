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
			void cmd_select(const Command& cmd);
			void cmd_listchars(const Command& cmd);
			void cmd_addroom(const Command& cmd);
			void cmd_makechar(const Command& cmd);
		
			void cmd_say(const Command& cmd);
			void cmd_where(const Command& cmd);
			void cmd_move(const Command& cmd);
			void cmd_look(const Command& cmd);
		
			RPGGame* _parent;
	};
	
}