#pragma once

#include <string>
#include <list>
#include <sstream>
#include "typedefs.h"

#include "stringutil.h"

namespace teh
{	
	struct Command
	{
		clientid client;
		stringvector arguments;
		char prefix;
	};
	
	class CommandHandlerInterface
	{
		public:
			virtual ~CommandHandlerInterface();
			virtual void handle_command(const Command& cmd) = 0;
			virtual bool accepts_command(const Command& cmd) = 0;
			virtual void handle_default(const Command& cmd);
			virtual bool accepts_default();
	};
	
	class CommandLexer
	{
		public:
			static const std::string PrefixChars;
			static char GetPrefix(const std::string& input);
		
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
			char _prefix;
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
