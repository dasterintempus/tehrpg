#include "gamecommandhandler.h"
#include "gameserver.h"
#include "mysql.h"

namespace teh
{

	GameCommandHandler::GameCommandHandler(GameServer* parent)
		: _parent(parent)
	{
		
	}

	void GameCommandHandler::handle_command(const Command& cmd)
	{
		GameClient* client = _parent->get_client(cmd.client);
		std::string first = cmd.arguments[0];
		switch (client->state())
		{
			case GameClient::WelcomeState:
				if (first == "login" && cmd.slashed)
				{
					if (cmd.arguments.size() != 2)
					{
						client->write_line("Invalid arguments for /login");
						return;
					}
					
					if (_parent->find_from_username(cmd.arguments[1]) == -1)
					{
						client->username(cmd.arguments[1]);
						client->state(GameClient::LoginState);
						client->write_line("Challenge:");
						client->write_line(client->challenge());
					}
					else
					{
						client->write_line("User already logged in.");
					}
				}
				else if (first == "register" && cmd.slashed)
				{
					if (cmd.arguments.size() != 3)
					{
						client->write_line("Invalid arguments for /register");
						return;
					}
					
					if (_parent->sql()->register_user(cmd.arguments[1], cmd.arguments[2], 1))
					{
						client->username(cmd.arguments[1]);
						client->state(GameClient::LoggedInState);
						client->write_line("Registered and logged in.");
					}
					
				}
				else if (first == "su" && cmd.slashed)
				{
					if (cmd.client == 0)
					{
						client->username("root");
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
				break;
			case GameClient::LoginState:
				if (first == "passwd" && cmd.slashed)
				{
					if (cmd.arguments.size() != 2)
					{
						client->write_line("Invalid arguments for /passwd");
						return;
					}
					
					if (_parent->sql()->validate_login(client->username(), client->challenge(), cmd.arguments[1]))
					{
						client->state(GameClient::LoggedInState);
						_parent->update_permissions(client);
						client->write_line("Logged in.");
					}
					else
					{
						client->generate_challenge();
						client->state(GameClient::WelcomeState);
						client->username("");
						client->write_line("Failed login, please /login again.");
					}
				}
				else
				{
					client->write_line("Invalid command for present state");
				}
				break;
			case GameClient::LoggedInState:
			case GameClient::PlayingState:
				if (first == "logout" && cmd.slashed)
				{
					if (cmd.arguments.size() != 1)
					{
						client->write_line("Invalid arguments for /logout");
						return;
					}
					client->generate_challenge();
					client->state(GameClient::WelcomeState);
					client->username("");
					client->write_line("Logged out.");
				}
				else if (first == "kill" && cmd.slashed)
				{
					//Check permissions
					if (client->permissions() & GameClient::ServerAdminPermissions)
					{
						if (cmd.arguments.size() != 3)
						{
							client->write_line("Invalid arguments for /kill");
							client->write_line("Usage: /kill {id|user} {target}");
							return;
						}
						if (cmd.arguments[1] == "id")
						{
							std::stringstream conv;
							conv << cmd.arguments[2];
							clientid killid;
							conv >> killid;
							if (_parent->kill(killid, client->username()))
							{
								client->write_line("Killed #" + conv.str());
							}
							else
							{
								client->write_line("Couldn't find #" + conv.str());
							}
						}
						else if (cmd.arguments[1] == "user")
						{
							std::string username = cmd.arguments[2];
							
							if (_parent->kill(username, client->username()))
							{
								client->write_line("Killed " + username);
							}
							else
							{
								client->write_line("Couldn't find " + username);
							}
						}
						else
						{
							client->write_line("Invalid argument for kill command.\nUsage: /kill {id|user} {target}");
						}
					}
					else
					{
						client->write_line("Insufficient permissions");
					}
				}
				else if (first == "shutdown" && cmd.slashed)
				{
					//Check permissions, or be console client
					if (client->permissions() & GameClient::RootPermissions || cmd.client == 0)
					{
						_parent->shutdown(client->username());
						client->write_line("Initiating shutdown");
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
				break;
			default:
				break;
		}
	}
	
	bool GameCommandHandler::accepts_command(const Command& cmd)
	{
		std::string first = cmd.arguments[0];
		if (cmd.slashed)
		{
			if (first == "login" ||
			first == "passwd" ||
			first == "logout" ||
			first == "register" ||
			first == "kill" ||
			first == "shutdown" ||
			first == "su")
				return true;
		}
		return false;
	}
		
}