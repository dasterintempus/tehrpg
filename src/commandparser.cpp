#include "commandparser.h"

namespace teh
{
	std::string stringjoin(const stringvector& list, const std::string& sep)
	{
		std::stringstream out;
		for (unsigned int n=0;n<list.size();n++)
		{
			if (n == list.size() - 1)
			{
				out << list[n];
			}
			else
			{
				out << list[n] << sep;
			}
		}
		return out.str();
	}
	
	CommandHandlerInterface::~CommandHandlerInterface()
	{
		
	}

	const std::string CommandLexer::PrefixChars = "\"'./:;!@#";
	
	char CommandLexer::GetPrefix(const std::string& input)
	{
		for (unsigned int n=0;n<PrefixChars.size();n++)
		{
			if (input[0] == PrefixChars[n])
			{
				return input[0];
			}
		}
		return '\0';
	}
	
	Command CommandLexer::lex(const std::string& line, const clientid& client)
	{
		CommandLexer lexer(client);
		lexer.input(line);
		return lexer.output();
	}
	
	CommandLexer::CommandLexer(const clientid& client)
		: _state(CommandLexer::NormalParseState), _prefix('\0'), _client(client), _valid(true)
	{
		
	}
	
	void CommandLexer::input(const std::string& in)
	{
		if (_valid)
		{
			if (_arguments.size() == 0)
			{
				char maybeprefix = CommandLexer::GetPrefix(in);
				if (maybeprefix != '\0')
				{
					_prefix = maybeprefix;
					update(in.substr(1));
				}
				else
				{
					update(in);
				}
			}
			else
			{
				update(in);
			}
		}
	}
	
	Command CommandLexer::output()
	{
		Command cmd;
		if (_valid)
		{
			cmd.arguments = _arguments;
			cmd.prefix = _prefix;
			cmd.client = _client;
			_valid = false;
		}
		else
		{
			cmd.prefix = _prefix;
			cmd.client = -1;
		}
		return cmd;
	}
	
	void CommandLexer::update(const std::string& in)
	{
		for (unsigned int n=0;n<in.size();n++)
		{
			char c = in[n];
			switch (_state)
			{
				case EscapedParseState:
					_buffer << c;
					_state = NormalParseState;
					break;
				case NormalParseState:
					switch (c)
					{
						case '\\':
							_state = EscapedParseState;
							break;
						case '\'':
							_state = QuotedParseState;
							//pop_buffer();
							break;
						case '"':
							_state = DoubleQuotedParseState;
							//pop_buffer();
							break;
						case ' ':
							pop_buffer();
							break;
						default:
							_buffer << c;
					}
					break;
				case QuotedParseState:
					switch (c)
					{
						case '\\':
							_state = EscapedParseState;
							break;
						case '\'':
							_state = NormalParseState;
							//pop_buffer();
							break;
						default:
							_buffer << c;
					}
					break;
				case DoubleQuotedParseState:
					switch (c)
					{
						case '\\':
							_state = EscapedParseState;
							break;
						case '"':
							_state = NormalParseState;
							//pop_buffer();
							break;
						default:
							_buffer << c;
					}
					break;
			}
		}
		pop_buffer();
	}
	
	void CommandLexer::pop_buffer()
	{
		if (_buffer.str().size() > 0)
			_arguments.push_back(_buffer.str());
		_buffer.str("");
	}
	
	CommandParser::CommandParser()
	{
		
	}
	
	CommandParser::~CommandParser()
	{
		for (std::list<CommandHandlerInterface*>::iterator i = _handlers.begin(); i != _handlers.end(); i++)
		{
			delete *i;
		}
	}
	
	void CommandParser::add_handler(CommandHandlerInterface* handler)
	{
		_handlers.push_back(handler);
	}
	
	void CommandParser::parse(const std::string& line, const clientid& client)
	{
		Command cmd = CommandLexer::lex(line, client);
		
		if (cmd.arguments.size() == 0)
			return;
		
		for (std::list<CommandHandlerInterface*>::iterator i = _handlers.begin(); i != _handlers.end(); i++)
		{
			CommandHandlerInterface* handler = *i;
			if (handler->accepts_command(cmd))
			{
				handler->handle_command(cmd);
				//return;
			}
		}
	}
}