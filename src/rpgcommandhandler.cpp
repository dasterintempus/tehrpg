#include "rpgcommandhandler.h"
#include "rpggame.h"
#include "rpgcharacter.h"
#include "rpgroom.h"
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
		if (first == "select" && cmd.slashed && !character)
		{
			if (cmd.arguments.size() != 2)
			{
				_parent->message_client(cmd.client, "Invalid usage of select command\nUsage: /select {charactername}");
			}
			else
			{
				if (_parent->check_logged_in(cmd.arguments[1]) == -1)
				{
					if (_parent->select_character(cmd.client, cmd.arguments[1]))
					{
						_parent->message_client(cmd.client, "Selected character: " + cmd.arguments[1]);
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
			return;
		}
		
		if (first == "listchars" && cmd.slashed && !character)
		{
			stringvector charnames = _parent->character_names(cmd.client);
			_parent->message_client(cmd.client, "Characters:");
			for (unsigned int n = 0;n < charnames.size();n++)
			{
				_parent->message_client(cmd.client, charnames[n]);
			}
			_parent->message_client(cmd.client, "---");
			return;
		}
		
		if (first == "addroom" && cmd.slashed)
		{
			GameClient* gc = _parent->get_client(cmd.client);
			std::cerr << "client perms: " << gc->permissions() << std::endl;
			std::cerr << "needed perms: " << GameClient::ContentAdminPermissions << std::endl;
			if ((gc->permissions() & GameClient::ContentAdminPermissions) == GameClient::ContentAdminPermissions)
			{
				if (cmd.arguments.size() == 5 && is_numeric<long int>(cmd.arguments[1]) && is_numeric<long int>(cmd.arguments[2]) && is_numeric<short int>(cmd.arguments[3]))
				{
					long int x = to_numeric<long int>(cmd.arguments[1]);
					long int y = to_numeric<long int>(cmd.arguments[2]);
					short int z = to_numeric<short int>(cmd.arguments[3]);
					std::string description = cmd.arguments[4];
					RPGRoom* room = RPGRoom::build(_parent, x, y, z, description);
					if (room)
					{
						_parent->message_client(cmd.client, "Room created.");
					}
					else
					{
						_parent->message_client(cmd.client, "Error creating room.");
					}
				}
				else
				{
					_parent->message_client(cmd.client, "Invalid arguments for /addroom.");
					_parent->message_client(cmd.client, "Usage: /addroom {x} {y} {z} {description}");
				}
			}
			else
			{
				_parent->message_client(cmd.client, "Insufficient permissions.");
			}
			return;
		}
		
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
		else if (first == "where")
		{
			RPGRoom* location = character->get_location();
			if (!location)
			{
				_parent->message_client(cmd.client, "Error! You are nowhere.");
			}
			else
			{
				std::stringstream sstream;
				sstream << "You are at X: " << location->xpos() << " Y: " << location->ypos() << " Z: " << location->zpos();
				_parent->message_client(cmd.client, sstream.str());
			}
		}
		else if (first == "move")
		{
			RPGRoom* destination = 0;
			if (cmd.arguments.size() != 2)
			{
				_parent->message_client(cmd.client, "Invalid usage of move command.");
				_parent->message_client(cmd.client, "Usage: move {up|down|east|west|north|south}");
				return;
			}
			else if (cmd.arguments[1] == "up")
			{
				destination = character->move(2, -1);
			}
			else if (cmd.arguments[1] == "down")
			{
				destination = character->move(2, 1);
			}
			else if (cmd.arguments[1] == "east")
			{
				destination = character->move(0, -1);
			}
			else if (cmd.arguments[1] == "west")
			{
				destination = character->move(0, 1);
			}
			else if (cmd.arguments[1] == "north")
			{
				destination = character->move(1, -1);
			}
			else if (cmd.arguments[1] == "south")
			{
				destination = character->move(0, 1);
			}
			else
			{
				_parent->message_client(cmd.client, "Invalid usage of move command.");
				_parent->message_client(cmd.client, "Usage: move {up|down|east|west|north|south}");
				return;
			}
			
			if (destination)
			{
				_parent->message_client(cmd.client, "Moved " + cmd.arguments[1]);
				_parent->message_client(cmd.client, "You are in: " + destination->description());
			}
			else
			{
				_parent->message_client(cmd.client, "Unable to move that direction right now. (Room does not exist)");
			}
		}
	}
	
	bool RPGCommandHandler::accepts_command(const Command& cmd)
	{
		std::string first = cmd.arguments[0];
		if (!cmd.slashed)
		{
			if (first == "say" ||
				first == "where" ||
				first == "move")
				return true;
		}
		else
		{
			if (first == "select" ||
				first == "listchars" ||
				first == "addroom")
				return true;
		}
		return false;
	}
}
