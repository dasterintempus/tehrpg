#include "rpgcommandhandler.h"
#include "rpgengine.h"
#include "rpgentity.h"
//#include "rpgcharacter.h"
//#include "rpgtile.h"
#include "mysql.h"
#include "gameserver.h"
//#include "rpginventory.h"
//#include "rpgitemtype.h"
//#include "rpgiteminstance.h"
#include "rpgentityfactory.h"
#include "rpgsystem.h"

#include <sstream>
/*

namespace teh
{
namespace RPG
{
	CommandHandler::CommandHandler(Engine* engine)
		: _engine(engine)
	{
		
	}
	
	void CommandHandler::handle_command(const Command& cmd)
	{
		Entity* character = _engine->get_pc(cmd.client);
		
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
		else if (first == "summonrock" && character)
		{
			cmd_summonrock(cmd);
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
		else if (first == "pickup" || first == "get")
		{
			cmd_pickup(cmd);
			return;
		}
		else if (first == "drop")
		{
			cmd_drop(cmd);
			return;
		}
		else if (first == "inventory" || cmd.prefix == '@')
		{
			cmd_inventory(cmd);
			return;
		}
		else if (first == "examine")
		{
			cmd_examine(cmd);
			return;
		}
	}
	
	bool CommandHandler::accepts_command(const Command& cmd)
	{
		std::string first = cmd.arguments[0];
		if (cmd.prefix == '\0')
		{
			if (first == "say" ||
				first == "where" ||
				first == "move" ||
				first == "look" ||
				first == "emote" ||
				first == "pickup" ||
				first == "get" ||
				first == "drop" ||
				first == "inventory" ||
				first == "examine")
				return true;
		}
		else if (cmd.prefix == '/')
		{
			if (first == "select" ||
				first == "listchars" ||
				first == "addtile" ||
				first == "makechar" ||
				first == "logout" ||
				first == "summonrock")
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
		else if (cmd.prefix == '@')
		{
			//inventory alias
			return true;
		}
		return false;
	}
	
	void CommandHandler::handle_default(const Command& cmd)
	{
		Entity* character = _engine->get_pc(cmd.client);
		if (!character)
			return;
		
		System* parsesystem = _engine->get_system("commandparse");
		bool ok = false;
		std::string output = parsesystem->process_command(character, cmd, ok);
		if (!ok && output == "")
		{
			_engine->message_client(cmd.client, "Failure: Unrecognized command.");
		}
		else if (!ok && output != "")
		{
			_engine->message_client(cmd.client, "Failure: " + output);
		}
		else if (ok && output != "")
		{
			_engine->message_client(cmd.client, "Success: " + output);
		}
	}
	
	bool CommandHandler::accepts_default()
	{
		return true;
	}
	
	void CommandHandler::cmd_select(const Command& cmd)
	{
		
		Entity* character = _engine->get_pc(cmd.client);
		if (character)
		{
			_engine->message_client(cmd.client, "You are already playing as " + character->name());
			return;
		}
		
		if (cmd.arguments.size() != 2)
		{
			_engine->message_client(cmd.client, "Invalid usage of select command\nUsage: /select {charactername}");
		}
		else
		{
			if (!_engine->is_pc_name_active(cmd.arguments[1]))
			{
				bool result = _engine->set_pc_name_played_by(cmd.arguments[1], cmd.client);
				if (result)
				{
					_engine->message_client(cmd.client, "Selected character: " + cmd.arguments[1]);
					//Tile* location = character->get_location();
					//location->broadcast(character->name() + " appears magically!");
				}
				else
				{
					_engine->message_client(cmd.client, "Unable to find that character on your account.");
				}
			}
			else
			{
				//This should be impossible, as users can only login once. Planning for a future possibility.
				_engine->message_client(cmd.client, "Character is already logged in.");
			}
		}
		
	}
	
	void CommandHandler::cmd_listchars(const Command& cmd)
	{
		
		Entity* character = _engine->get_pc(cmd.client);
		
		std::vector<std::string> charnames = _engine->get_character_names_of_client(cmd.client);
		_engine->message_client(cmd.client, "Characters:");
		for (unsigned int n = 0;n < charnames.size();n++)
		{
			if (character && character->name() == charnames[n])
			{
				_engine->message_client(cmd.client, "*" + charnames[n]);
			}
			else
			{
				_engine->message_client(cmd.client, charnames[n]);
			}
		}
		_engine->message_client(cmd.client, "---");
		
	}
	
	
	void CommandHandler::cmd_addtile(const Command& cmd)
	{
		GameClient* gc = _engine->get_client(cmd.client);
		if (!gc)
			return;
		
		if ((gc->permissions() & GameClient::ContentAdminPermissions) == GameClient::ContentAdminPermissions)
		{
			if (cmd.arguments.size() == 4 && is_numeric<long int>(cmd.arguments[1]) && is_numeric<long int>(cmd.arguments[2]))
			{
				long int x = to_numeric<long int>(cmd.arguments[1]);
				long int y = to_numeric<long int>(cmd.arguments[2]);
				std::string description = cmd.arguments[3];
				Tile* tile = Tile::build(_engine, x, y, false, description);
				if (tile)
				{
					_engine->message_client(cmd.client, "Tile created.");
				}
				else
				{
					_engine->message_client(cmd.client, "Error creating tile.");
				}
			}
			else
			{
				_engine->message_client(cmd.client, "Invalid arguments for /addtile.");
				_engine->message_client(cmd.client, "Usage: /addtile {x} {y} {description}");
			}
		}
		else
		{
			_engine->message_client(cmd.client, "Insufficient permissions.");
		}
	}
	
	
	void CommandHandler::cmd_makechar(const Command& cmd)
	{
		
		Entity* character = _engine->get_pc(cmd.client);
		if (character)
			return;
		
		GameClient* gc = _engine->get_client(cmd.client);
		if (!gc)
			return;
		
		unsigned int userid = gc->userid();
		
		if (cmd.arguments.size() != 2)
		{
			_engine->message_client(cmd.client, "Invalid usage of /makechar command.");
			_engine->message_client(cmd.client, "Usage: /makechar {name}");
			return;
		}
		
		std::string charname = cmd.arguments[1];
		
		std::pair<long int, long int> spawn = _engine->globalspawnpoint();
		character = constructPlayerCharacter(_engine, spawn.first, spawn.second, charname, userid);
		if (character)
		{
			_engine->message_client(cmd.client, "Character created.");
			return;
		}
		else
		{
			_engine->message_client(cmd.client, "Unable to create character (name taken?)");
			return;
		}
		
	}
	
	void CommandHandler::cmd_logout(const Command& cmd)
	{
		_engine->logout(cmd.client);
	}
	
	
	void CommandHandler::cmd_summonrock(const Command& cmd)
	{
		Character* character = _engine->get_active_character(cmd.client);
		if (!character)
			return;
		
		Tile* location = character->get_location();
		
		Inventory* ground = location->get_inventory();
		
		ItemType* rock = _engine->find_itemtype("rock");
		
		ItemInstance* instance = ItemInstance::build(_engine, ground, rock);
		if (instance)
		{
			location->broadcast(character->name() + " casts Summon Rock! A rock appears.");
		}
		else
		{
			location->broadcast(character->name() + " casts Summon Rock! ...But it failed.");
		}
	}
	
	
	
	
	void CommandHandler::cmd_say(const Command& cmd)
	{
		Character* character = _engine->get_active_character(cmd.client);
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
	
	void CommandHandler::cmd_where(const Command& cmd)
	{	
		Character* character = _engine->get_active_character(cmd.client);
		if (!character)
			return;
		
		Tile* location = character->get_location();
		if (!location)
		{
			_engine->message_client(cmd.client, "Error! You are nowhere.");
		}
		else
		{
			std::stringstream sstream;
			sstream << "You are at X: " << location->xpos() << " Y: " << location->ypos();
			_engine->message_client(cmd.client, sstream.str());
		}
	}
	
	void CommandHandler::cmd_move(const Command& cmd)
	{
		Character* character = _engine->get_active_character(cmd.client);
		if (!character)
			return;
		
		Tile* destination = 0;
		if (cmd.arguments.size() != 2)
		{
			_engine->message_client(cmd.client, "Invalid usage of move command.");
			_engine->message_client(cmd.client, "Usage: move {east|west|north|south}");
			return;
		}
		else if (cmd.arguments[1] == "east" || cmd.arguments[1] == "west" || cmd.arguments[1] == "north" || cmd.arguments[1] == "south")
		{
			destination = character->move(cmd.arguments[1]);
		}
		else
		{
			_engine->message_client(cmd.client, "Invalid usage of move command.");
			_engine->message_client(cmd.client, "Usage: move {east|west|north|south}");
			return;
		}
		
		if (destination)
		{
			_engine->message_client(cmd.client, "Moved " + cmd.arguments[1]);
			std::string looktext = character->look();
			_engine->message_client(cmd.client, looktext);
		}
		else
		{
			_engine->message_client(cmd.client, "Unable to move that direction.");
		}
	}
	
	void CommandHandler::cmd_look(const Command& cmd)
	{
		Character* character = _engine->get_active_character(cmd.client);
		if (!character)
			return;
		
		if (cmd.arguments.size() != 1)
		{
			_engine->message_client(cmd.client, "Invalid usage of look command.");
			_engine->message_client(cmd.client, "Usage: look");
			return;
		}
		
		std::string looktext = character->look();
		_engine->message_client(cmd.client, looktext);
	}
	
	void CommandHandler::cmd_emote(const Command& cmd)
	{
		Character* character = _engine->get_active_character(cmd.client);
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
	
	void CommandHandler::cmd_pickup(const Command& cmd)
	{
		Character* character = _engine->get_active_character(cmd.client);
		if (!character)
			return;
		
		if (cmd.arguments.size() == 4)
		{
			std::string target = cmd.arguments[1];
			if (!is_numeric<unsigned int>(cmd.arguments[2]))
			{
				_engine->message_client(cmd.client, "Invalid usage of pickup command.");
				_engine->message_client(cmd.client, "Usage: pickup {itemname} [#] [inventoryname]");
				return;
			}
			unsigned int n = to_numeric<unsigned int>(cmd.arguments[2]);
			std::string destination = cmd.arguments[3];
			
			std::string message = character->pickup(target, n, destination);
			_engine->message_client(cmd.client, message);
		}
		else if (cmd.arguments.size() == 3)
		{
			std::string target = cmd.arguments[1];
			if (!is_numeric<unsigned int>(cmd.arguments[2]))
			{
				unsigned int n = 1;
				std::string destination = cmd.arguments[2];
				std::string message = character->pickup(target, n, destination);
				_engine->message_client(cmd.client, message);
			}
			else
			{
				unsigned int n = to_numeric<unsigned int>(cmd.arguments[2]);
				std::string message = character->pickup(target, n);
				_engine->message_client(cmd.client, message);
			}
		}
		else if (cmd.arguments.size() == 2)
		{
			std::string target = cmd.arguments[1];
			std::string message = character->pickup(target);
			_engine->message_client(cmd.client, message);
		}
		else
		{
			_engine->message_client(cmd.client, "Invalid usage of pickup command.");
			_engine->message_client(cmd.client, "Usage: pickup {itemname} [#] [inventoryname]");
			return;
		}
	}
	
	void CommandHandler::cmd_drop(const Command& cmd)
	{
		Character* character = _engine->get_active_character(cmd.client);
		if (!character)
			return;
		
		if (cmd.arguments.size() == 4)
		{
			std::string target = cmd.arguments[1];
			if (!is_numeric<unsigned int>(cmd.arguments[2]))
			{
				_engine->message_client(cmd.client, "Invalid usage of drop command.");
				_engine->message_client(cmd.client, "Usage: drop {itemname} [#] [inventoryname]");
				return;
			}
			unsigned int n = to_numeric<unsigned int>(cmd.arguments[2]);
			std::string destination = cmd.arguments[3];
			
			std::string message = character->drop(target, n, destination);
			_engine->message_client(cmd.client, message);
		}
		else if (cmd.arguments.size() == 3)
		{
			std::string target = cmd.arguments[1];
			if (!is_numeric<unsigned int>(cmd.arguments[2]))
			{
				unsigned int n = 1;
				std::string destination = cmd.arguments[2];
				std::string message = character->drop(target, n, destination);
				_engine->message_client(cmd.client, message);
			}
			else
			{
				unsigned int n = to_numeric<unsigned int>(cmd.arguments[2]);
				std::string message = character->drop(target, n);
				_engine->message_client(cmd.client, message);
			}
		}
		else if (cmd.arguments.size() == 2)
		{
			std::string target = cmd.arguments[1];
			std::string message = character->drop(target);
			_engine->message_client(cmd.client, message);
		}
		else
		{
			_engine->message_client(cmd.client, "Invalid usage of drop command.");
			_engine->message_client(cmd.client, "Usage: drop {itemname} [#] [inventoryname]");
			return;
		}
	}
	
	void CommandHandler::cmd_inventory(const Command& cmd)
	{
		Character* character = _engine->get_active_character(cmd.client);
		if (!character)
			return;
		
		std::string invname = "";
		if (cmd.arguments.size() == 2 && cmd.prefix == '\0')
		{
			invname = cmd.arguments[1];
		}
		else if (cmd.arguments.size() == 1 && cmd.prefix == '@')
		{
			invname = cmd.arguments[0];
		}
		else if (cmd.arguments.size() != 1 && cmd.prefix == '@')
		{
			_engine->message_client(cmd.client, "Invalid usage of inventory command.");
			_engine->message_client(cmd.client, "Usage: inventory [inventoryname]");
			_engine->message_client(cmd.client, "Usage: @{inventoryname}");
			return;
		}
		
		if (invname != "")
		{
			Inventory* inv = character->get_inventory(invname);
			if (!inv)
			{
				_engine->message_client(cmd.client, "You don't have an inventory named '" + invname + "'.");
				return;
			}
			std::stringstream message;
			message << "You look in your inventory '" << invname << "':";
			message << inv->describe_contents() << "\n";
			message << "It has " << inv->space_remaining() << " cubic meters of space left.\n";
			message << "You are carrying " << character->carrying_mass() << "/" << character->max_carrying_mass() << " kg.";
			_engine->message_client(cmd.client, message.str());
		}
		else
		{
			std::stringstream message;
			std::vector<Inventory*> inventories = character->all_inventories();
			for (unsigned int n = 0;n < inventories.size();n++)
			{
				message << "You look in your inventory '" << inventories[n]->name() << "': ";
				message << inventories[n]->describe_contents() << "\n";
				message << "It has " << inventories[n]->space_remaining() << " cubic meters of space left.\n";
			}
			message << "You are carrying " << character->carrying_mass() << "/" << character->max_carrying_mass() << " kg.";
			_engine->message_client(cmd.client, message.str());
		}
	}
	
	void CommandHandler::cmd_examine(const Command& cmd)
	{
		Character* character = _engine->get_active_character(cmd.client);
		if (!character)
			return;
		
		if (cmd.arguments.size() == 4)
		{
			std::string origin = cmd.arguments[1];
			std::string target = cmd.arguments[2];
			if (!is_numeric<unsigned int>(cmd.arguments[3]))
			{
				_engine->message_client(cmd.client, "Invalid usage of examine command.");
				_engine->message_client(cmd.client, "Usage: examine {'ground' | inventoryname} {itemname} [#]");
				return;
			}
			unsigned int n = to_numeric<unsigned int>(cmd.arguments[3]);
			
			std::string message = character->examine(origin, target, n);
			_engine->message_client(cmd.client, message);
		}
		else if (cmd.arguments.size() == 3)
		{
			std::string origin = cmd.arguments[1];
			if (!is_numeric<unsigned int>(cmd.arguments[2]))
			{
				std::string target = cmd.arguments[2];
				std::string message = character->examine(origin, target);
				_engine->message_client(cmd.client, message);
			}
			else
			{
				_engine->message_client(cmd.client, "Invalid usage of examine command.");
				_engine->message_client(cmd.client, "Usage: examine {'ground' | inventoryname} {itemname} [#]");
				return;
			}
		}
		else
		{
			_engine->message_client(cmd.client, "Invalid usage of examine command.");
			_engine->message_client(cmd.client, "Usage: examine {'ground' | inventoryname} {itemname} [#]");
			return;
		}
	}
	
}
}
*/