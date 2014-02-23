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
			void handle_default(const Command& cmd);
			bool accepts_default();
		private:
			void cmd_select(const Command& cmd);
			void cmd_listchars(const Command& cmd);
			void cmd_addtile(const Command& cmd);
			void cmd_makechar(const Command& cmd);
			void cmd_logout(const Command& cmd);
		
			void cmd_say(const Command& cmd);
			void cmd_where(const Command& cmd);
			void cmd_move(const Command& cmd);
			void cmd_look(const Command& cmd);
			void cmd_emote(const Command& cmd);
		
			RPGGame* _parent;
	};
	
}