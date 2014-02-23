#include "rpgcommandhandler.h"
#include "rpggame.h"
#include "rpgcharacter.h"
#include "rpgtile.h"
#include "mysql.h"
#include "gameserver.h"

namespace teh
{

	RPGCommandHandler::RPGCommandHandler(RPGGame* parent)
		: _parent(parent)
	{
		
	}
	
	void RPGCommandHandler::handle_command(const Command& cmd)
	{
		RPGCharacter* character = _parent->get_active_character(cmd.client);
		
		std::string first = cmd.arguments[0];
		if (first == "select" && !character)
		{
			cmd_select(cmd);
			return;
		}
		else if (first == "listchars")
		{
			cmd_listchars(cmd);
			return;
		}
		else if (first == "addtile")
		{
			cmd_addtile(cmd);
			return;
		}
		else if (first == "makechar" && !character)
		{
			cmd_makechar(cmd);
			return;
		}
		else if (first == "logout" && character)
		{
			cmd_logout(cmd);
			return;
		}
		
		if (!character)
			return;
		
		if (first == "say" || cmd.prefix == '.' || cmd.prefix == '"' || cmd.prefix == '\'')
		{
			cmd_say(cmd);
			return;
		}
		else if (first == "where")
		{
			cmd_where(cmd);
			return;
		}
		else if (first == "move")
		{
			cmd_move(cmd);
			return;
		}
		else if (first == "look")
		{
			cmd_look(cmd);
			return;
		}
		else if (first == "emote" || cmd.prefix == ':')
		{
			cmd_emote(cmd);
			return;
		}
	}
	
	bool RPGCommandHandler::accepts_command(const Command& cmd)
	{
		std::string first = cmd.arguments[0];
		if (cmd.prefix == '\0')
		{
			if (first == "say" ||
				first == "where" ||
				first == "move" ||
				first == "look" ||
				first == "emote")
				return true;
		}
		else if (cmd.prefix == '/')
		{
			if (first == "select" ||
				first == "listchars" ||
				first == "addtile" ||
				first == "makechar" ||
				first == "logout")
				return true;
		}
		else if (cmd.prefix == '.' || cmd.prefix == '"' || cmd.prefix == '\'')
		{
			//say aliases
			return true;
		}
		else if (cmd.prefix == ':')
		{
			//emote alias
			return true;
		}
		return false;
	}
	
	void RPGCommandHandler::handle_default(const Command& cmd)
	{
		RPGCharacter* character = _parent->get_active_character(cmd.client);
		if (!character)
			return;
		
		
		
	}
	
	bool RPGCommandHandler::accepts_default()
	{
		return true;
	}
	
	void RPGCommandHandler::cmd_select(const Command& cmd)
	{
		RPGCharacter* character = _parent->get_active_character(cmd.client);
		if (character)
		{
			_parent->message_client(cmd.client, "You are already playing as " + character->name());
			return;
		}
		
		if (cmd.arguments.size() != 2)
		{
			_parent->message_client(cmd.client, "Invalid usage of select command\nUsage: /select {charactername}");
		}
		else
		{
			if (_parent->check_logged_in(cmd.arguments[1]) == -1)
			{
				character = _parent->select_character(cmd.client, cmd.arguments[1]);
				if (character)
				{
					_parent->message_client(cmd.client, "Selected character: " + cmd.arguments[1]);
					RPGTile* location = character->get_location();
					location->broadcast(character->name() + " appears magically!");
				}
				else
				{
					_parent->message_client(cmd.client, "Unable to find that character on your account.");
				}
			}
			else
			{
				//This should be impossible, as users can only login once. Planning for a future possibility.
				_parent->message_client(cmd.client, "Character is already logged in.");
			}
		}
	}
	
	void RPGCommandHandler::cmd_listchars(const Command& cmd)
	{
		RPGCharacter* character = _parent->get_active_character(cmd.client);
		
		stringvector charnames = _parent->character_names(cmd.client);
		_parent->message_client(cmd.client, "Characters:");
		for (unsigned int n = 0;n < charnames.size();n++)
		{
			if (character && character->name() == charnames[n])
			{
				_parent->message_client(cmd.client, "*" + charnames[n]);
			}
			else
			{
				_parent->message_client(cmd.client, charnames[n]);
			}
		}
		_parent->message_client(cmd.client, "---");
	}
	
	void RPGCommandHandler::cmd_addtile(const Command& cmd)
	{
		GameClient* gc = _parent->get_client(cmd.client);
		if (!gc)
			return;
		
		if ((gc->permissions() & GameClient::ContentAdminPermissions) == GameClient::ContentAdminPermissions)
		{
			if (cmd.arguments.size() == 4 && is_numeric<long int>(cmd.arguments[1]) && is_numeric<long int>(cmd.arguments[2]))
			{
				long int x = to_numeric<long int>(cmd.arguments[1]);
				long int y = to_numeric<long int>(cmd.arguments[2]);
				std::string description = cmd.arguments[3];
				RPGTile* tile = RPGTile::build(_parent, x, y, false, description);
				if (tile)
				{
					_parent->message_client(cmd.client, "Tile created.");
				}
				else
				{
					_parent->message_client(cmd.client, "Error creating tile.");
				}
			}
			else
			{
				_parent->message_client(cmd.client, "Invalid arguments for /addtile.");
				_parent->message_client(cmd.client, "Usage: /addtile {x} {y} {description}");
			}
		}
		else
		{
			_parent->message_client(cmd.client, "Insufficient permissions.");
		}
	}
	
	void RPGCommandHandler::cmd_makechar(const Command& cmd)
	{
		RPGCharacter* character = _parent->get_active_character(cmd.client);
		if (character)
			return;
		
		GameClient* gc = _parent->get_client(cmd.client);
		if (!gc)
			return;
		
		std::string username = gc->username();
		
		if (cmd.arguments.size() != 2 && cmd.arguments.size() != 4)
		{
			_parent->message_client(cmd.client, "Invalid usage of /makechar command.");
			_parent->message_client(cmd.client, "Usage: /makechar {name} [+{stat} -{stat}]");
			return;
		}
		
		std::string charname = cmd.arguments[1];
		
		std::map<std::string, unsigned short int> stats;
		for (unsigned int n = 0;n < 6;n++)
		{
			stats[RPGCharacter::StatNames[n]] = 10;
		}
		
		if (cmd.arguments.size() == 4)
		{
			if (cmd.arguments[2][0] != '+' || cmd.arguments[3][0] != '-')
			{
				_parent->message_client(cmd.client, "Invalid usage of /makechar command.");
				_parent->message_client(cmd.client, "Usage: /makechar {name} [+{stat} -{stat}]");
				return;
			}
			std::string upstat = cmd.arguments[2].substr(1);
			std::string downstat = cmd.arguments[3].substr(1);
			std::string upstatreal;
			std::string downstatreal;
			for (unsigned int n = 0;n < 6;n++)
			{
				std::string statname = RPGCharacter::StatNames[n];
				if (statname.substr(0, upstat.size()) == upstat)
				{
					upstatreal = statname;
				}
				if (statname.substr(0, downstat.size()) == downstat)
				{
					downstatreal = statname;
				}
			}
			if (upstatreal == "" || downstatreal == "")
			{
				_parent->message_client(cmd.client, "Unable to match stat names.");
				_parent->message_client(cmd.client, "Invalid usage of /makechar command.");
				_parent->message_client(cmd.client, "Usage: /makechar {name} [+{stat} -{stat}]");
				return;
			}
			
			if (upstatreal == downstatreal)
			{
				_parent->message_client(cmd.client, "You can't boost and lower the same stat.");
				_parent->message_client(cmd.client, "Invalid usage of /makechar command.");
				_parent->message_client(cmd.client, "Usage: /makechar {name} [+{stat} -{stat}]");
				return;
			}
			
			stats[upstatreal] += 2;
			stats[downstatreal] -= 2;
		}
		
		RPGTile* tile = _parent->get_tile(1);
		
		character = RPGCharacter::build(_parent, charname, username, tile, stats);
		if (character)
		{
			_parent->message_client(cmd.client, "Character created.");
			return;
		}
		else
		{
			_parent->message_client(cmd.client, "Unable to create character (name taken?)");
			return;
		}
	}
	
	void RPGCommandHandler::cmd_logout(const Command& cmd)
	{
		_parent->logout(cmd.client);
	}

	void RPGCommandHandler::cmd_say(const Command& cmd)
	{
		RPGCharacter* character = _parent->get_active_character(cmd.client);
		if (!character)
			return;
		
		std::string msg;
		
		if (cmd.arguments[0] == "say" && cmd.prefix == '\0')
		{
			stringvector args = cmd.arguments;
			args.erase(args.begin());
			msg = stringjoin(args);
		}
		else
		{
			msg = stringjoin(cmd.arguments);
		}
		
		static const boost::regex smile("(.+) +[:=]\\) *$");
		static const std::string smile_replacer = "smiles, and says: \"\\1\"";
		
		std::string emotemsg;
		
		if (boost::regex_match(msg, smile))
		{
			emotemsg = boost::regex_replace(msg, smile, smile_replacer, boost::match_default | boost::format_sed);
		}
		
		if (emotemsg == "")
		{
			character->say(msg);
		}
		else
		{
			character->emote(emotemsg);
		}
	}
	
	void RPGCommandHandler::cmd_where(const Command& cmd)
	{	
		RPGCharacter* character = _parent->get_active_character(cmd.client);
		if (!character)
			return;
		
		RPGTile* location = character->get_location();
		if (!location)
		{
			_parent->message_client(cmd.client, "Error! You are nowhere.");
		}
		else
		{
			std::stringstream sstream;
			sstream << "You are at X: " << location->xpos() << " Y: " << location->ypos();
			_parent->message_client(cmd.client, sstream.str());
		}
	}
	
	void RPGCommandHandler::cmd_move(const Command& cmd)
	{
		RPGCharacter* character = _parent->get_active_character(cmd.client);
		if (!character)
			return;
		
		RPGTile* destination = 0;
		if (cmd.arguments.size() != 2)
		{
			_parent->message_client(cmd.client, "Invalid usage of move command.");
			_parent->message_client(cmd.client, "Usage: move {east|west|north|south}");
			return;
		}
		else if (cmd.arguments[1] == "east" || cmd.arguments[1] == "west" || cmd.arguments[1] == "north" || cmd.arguments[1] == "south")
		{
			destination = character->move(cmd.arguments[1]);
		}
		else
		{
			_parent->message_client(cmd.client, "Invalid usage of move command.");
			_parent->message_client(cmd.client, "Usage: move {east|west|north|south}");
			return;
		}
		
		if (destination)
		{
			_parent->message_client(cmd.client, "Moved " + cmd.arguments[1]);
			std::string looktext = character->look();
			_parent->message_client(cmd.client, looktext);
		}
		else
		{
			_parent->message_client(cmd.client, "Unable to move that direction.");
		}
	}
	
	void RPGCommandHandler::cmd_look(const Command& cmd)
	{
		RPGCharacter* character = _parent->get_active_character(cmd.client);
		if (!character)
			return;
		
		if (cmd.arguments.size() != 1)
		{
			_parent->message_client(cmd.client, "Invalid usage of look command.");
			_parent->message_client(cmd.client, "Usage: look");
			return;
		}
		
		std::string looktext = character->look();
		_parent->message_client(cmd.client, looktext);
	}
	
	void RPGCommandHandler::cmd_emote(const Command& cmd)
	{
		RPGCharacter* character = _parent->get_active_character(cmd.client);
		if (!character)
			return;
		
		if (cmd.arguments[0] == "emote" && cmd.prefix == '\0')
		{
			stringvector args = cmd.arguments;
			args.erase(args.begin());
			character->emote(stringjoin(args));
		}
		else if (cmd.arguments[0] == "s" && cmd.prefix == ':')
		{
			stringvector args = cmd.arguments;
			args.erase(args.begin());
			character->emote(stringjoin(args), true);
		}
		else
		{
			character->emote(stringjoin(cmd.arguments));
		}
	}
}
