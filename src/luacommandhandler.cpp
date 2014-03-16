#include "luacommandhandler.h"
#include "rpgengine.h"
#include "rpgsystem.h"

#include <iostream>

namespace teh
{
	namespace RPG
	{
		LuaCommandHandler::LuaCommandHandler(Engine* engine, const std::string& parser, char prefix)
			: _engine(engine), _prefix(prefix)
		{
			unsigned int pos = parser.find(':');
			_parsesystem = parser.substr(0, pos);
			_parsefunction = parser.substr(pos+1);
			std::cerr << _parsesystem << " " << _parsefunction << std::endl;
		}
		
		void LuaCommandHandler::handle_command(const Command& cmd)
		{
			Entity* character = _engine->get_pc(cmd.client);
			
			System* parsesystem = _engine->get_system(_parsesystem);
			bool ok = false;
			std::string output = parsesystem->process_command(character, cmd, _parsefunction, ok);
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
		
		bool LuaCommandHandler::accepts_command(const Command& cmd)
		{
			if (cmd.prefix == _prefix)
				return true;
			return false;
		}
	}
}