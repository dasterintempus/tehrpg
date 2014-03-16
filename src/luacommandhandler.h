#pragma once

#include "commandparser.h"

namespace teh
{
	namespace RPG
	{
		class Engine;
		
		class LuaCommandHandler
			: public CommandHandlerInterface
		{
			public:
				LuaCommandHandler(Engine* engine, const std::string& parser, char prefix);
				void handle_command(const Command& cmd);
				bool accepts_command(const Command& cmd);
			private:
				Engine* _engine;
				std::string _parsesystem;
				std::string _parsefunction;
				char _prefix;
		};
	}
}