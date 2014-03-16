#include "metagamecommandhandler.h"
#include "gameserver.h"
#include "mysql.h"
#include "rpgengine.h"

namespace teh
{

	MetagameCommandHandler::MetagameCommandHandler(GameServer* parent)
		: _parent(parent)
	{
		
	}

	void MetagameCommandHandler::handle_command(const Command& cmd)
	{
		GameClient* client = _parent->get_client(cmd.client);
		std::string first = cmd.arguments[0];
		if (client->state() == GameClient::WelcomeState)
		{
			if (first == "login")
			{
				if (cmd.arguments.size() != 2)
				{
					client->write_line("Invalid arguments for /login");
					return;
				}
				std::string username = cmd.arguments[1];
				unsigned int userid = _parent->sql()->get_userid(username);
				
				if (_parent->find_from_userid(userid) == -1)
				{
					client->userid(userid);
					client->state(GameClient::LoginState);
					client->write_line("Challenge:");
					client->write_line(client->challenge());
				}
				else
				{
					client->write_line("User already logged in.");
				}
			}
			else if (first == "su")
			{
				if (cmd.client == 0)
				{
					client->userid(-1);
					client->state(GameClient::LoggedInState);
					client->write_line("Welcome, root");
				}
				else
				{
					client->write_line("You are not logged in via console.");
				}
			}
			else
			{
				client->write_line("Invalid command for present state");
			}
		}
		else if (client->state() == GameClient::LoginState)
		{
			if (first == "passwd")
			{
				if (cmd.arguments.size() != 2)
				{
					client->write_line("Invalid arguments for /passwd");
					return;
				}
				
				if (_parent->sql()->validate_login(client->userid(), client->challenge(), cmd.arguments[1]))
				{
					client->state(GameClient::LoggedInState);
					_parent->update_permissions(client);
					client->write_line("Logged in.");
				}
				else
				{
					client->generate_challenge();
					client->state(GameClient::WelcomeState);
					client->userid(0);
					client->write_line("Failed login, please /login again.");
				}
			}
			else
			{
				client->write_line("Invalid command for present state");
			}
		}
		else if (client->state() == GameClient::LoggedInState || client->state() == GameClient::PlayingState)
		{
			if (first == "logout")
			{
				if (cmd.arguments.size() != 1)
				{
					client->write_line("Invalid arguments for /logout");
					return;
				}
				client->generate_challenge();
				client->state(GameClient::WelcomeState);
				client->userid(0);
				client->write_line("Logged out.");
			}
			else if (first == "createacct")
			{
				//Check permissions
				if (client->permissions() & GameClient::ServerAdminPermissions || cmd.client == 0)
				{
					if (cmd.arguments.size() != 3)
					{
						client->write_line("Invalid arguments for /createacct");
						client->write_line("Usage: /createacct {username} {password}");
						return;
					}
					
					if (_parent->sql()->register_user(cmd.arguments[1], cmd.arguments[2], 1))
					{
						client->write_line("Created account.");
					}
					else
					{
						client->write_line("Unable to create account, username taken?");
					}
				}
				else
				{
					client->write_line("Insufficient permissions");
				}
			}
			else if (first == "shutdown")
			{
				//Check permissions, or be console client
				if (client->permissions() & GameClient::RootPermissions || cmd.client == 0)
				{
					client->write_line("Initiating shutdown");
					_parent->shutdown(_parent->sql()->get_username(client->userid()));
				}
				else
				{
					client->write_line("Insufficient permissions");
				}
			}
			else if (first == "setperms")
			{
				//Check permissions, or be console client
				if (client->permissions() & GameClient::RootPermissions || cmd.client == 0)
				{
					if (cmd.arguments.size() != 3)
					{
						client->write_line("Invalid arguments for /setperms");
						client->write_line("Usage: /settperms {username} {permissions}");
						return;
					}
					unsigned int targetuser = _parent->sql()->get_userid(cmd.arguments[1]);
					if (_parent->sql()->set_permissions(targetuser, to_numeric<unsigned short int>(cmd.arguments[2])))
					{
						client->write_line("Permissions set.");
					}
					else
					{
						client->write_line("Unable to set permissions.");
					}
				}
				else
				{
					client->write_line("Insufficient permissions");
				}
			}
			else
			{
				client->write_line("Invalid command for present state");
			}
		}
	}
	
	bool MetagameCommandHandler::accepts_command(const Command& cmd)
	{
		std::string first = cmd.arguments[0];
		if (cmd.prefix == '/')
		{
			if (first == "login" ||
			first == "passwd" ||
			first == "logout" ||
			//first == "register" ||
			//first == "kill" ||
			first == "shutdown" ||
			first == "su" ||
			first == "setperms" ||
			first == "createacct" ||
			first == "firstinit")
				return true;
		}
		return false;
	}
		
}