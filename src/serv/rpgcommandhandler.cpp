#include "rpgcommandhandler.h"
#include "rpggame.h"
#include "rpgcharacter.h"
#include "rpgroom.h"
#include "mysql.h"

namespace teh
{

	RPGCommandHandler::RPGCommandHandler(RPGGame* parent)
		: _parent(parent)
	{
		
	}
	
	void RPGCommandHandler::handle_command(const Command& cmd)
	{
		std::string first = cmd.arguments[0];
		if (first == "select")
		{
			if (cmd.arguments.size() != 2)
			{
				_parent->message_client(cmd.client, "Invalid usage of select command\nUsage: /select {charactername}");
			}
			else
			{
				_parent->select_character(cmd.client, cmd.arguments[1]);
			}
			return;
		}
		
		RPGCharacter* character = _parent->get_active_character(cmd.client);
		if (!character)
			return;
		
		if (first == "say")
		{
			if (cmd.arguments.size() != 2)
			{
				_parent->message_client(cmd.client, "Invalid usage of say command (use double quotes)");
			}
			else
			{
				
				character->say(cmd.arguments[1]);
			}
		}
		else 
	}
	
	bool RPGCommandHandler::accepts_command(const Command& cmd)
	{
		std::string first = cmd.arguments[0];
		if (!cmd.slashed)
		{
			if (first == "say")
				return true;
		}
		else
		{
			if (first == "select")
				return true;
		}
		return false;
	}
}
