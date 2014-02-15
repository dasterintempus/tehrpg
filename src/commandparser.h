#pragma once

#include <string>
#include <list>
#include <sstream>
#include "typedefs.h"

namespace teh
{
	struct Command
	{
		clientid client;
		stringvector arguments;
		bool slashed;
	};
	
	class CommandHandlerInterface
	{
		public:
			virtual ~CommandHandlerInterface();
			virtual void handle_command(const Command& cmd) = 0;
			virtual bool accepts_command(const Command& cmd) = 0;
	};
	
	class CommandLexer
	{
		public:
			static Command lex(const std::string& line, const clientid& client);
			CommandLexer(const clientid& client);
			void input(const std::string& in);
			Command output();
		private:
			void update(const std::string& in);
			void pop_buffer();
			enum ParseState
			{
				EscapedParseState,
				NormalParseState,
				QuotedParseState,
				DoubleQuotedParseState,
			};
			
			bool _valid;
			ParseState _state;
			bool _slashed;
			std::stringstream _buffer;
			stringvector _arguments;
			clientid _client;
	};
	
	class CommandParser
	{
		public:
			CommandParser();
			~CommandParser();
			void add_handler(CommandHandlerInterface* handler);
			void parse(const std::string& line, const clientid& client);
		private:
			std::list<CommandHandlerInterface*> _handlers;
	};
}
