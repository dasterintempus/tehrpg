#pragma once

#include "commandparser.h"

namespace teh
{
namespace RPG
{
	class Game;
	
	class CommandHandler
		: public CommandHandlerInterface
	{
		public:
			CommandHandler(Game* parent);
			
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
			void cmd_summonrock(const Command& cmd);
			
			void cmd_say(const Command& cmd);
			void cmd_where(const Command& cmd);
			void cmd_move(const Command& cmd);
			void cmd_look(const Command& cmd);
			void cmd_emote(const Command& cmd);
			void cmd_pickup(const Command& cmd);
			void cmd_drop(const Command& cmd);
			void cmd_inventory(const Command& cmd);
			void cmd_examine(const Command& cmd);
		
			Game* _parent;
	};
	
}
}