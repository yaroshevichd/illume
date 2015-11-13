#include <Arduino.h>
#include "IllumeUtils.h"
#include <stdlib.h>

#define TOKEN_COMMAND_DELIMITER '$'
#define TOKEN_COMMAND_ARGS_DELIMITER ','
#define TOKEN_COMMAND_ARG_FIELD_DELIMITER ':'

enum ParserState
{
	ParserState_Begin,
	ParserState_ParseCommandType,
	ParserState_ParseCommandValue,
	ParserState_ParseCommandArgValue,
	ParserState_ParseCommandArgsCountValue,
	ParserState_Done
};


CommandType parseCommandType(const String& token)
{
    if (token.equals("save-cfg"))
        return CommandType_SaveCfg;
    else if (token.equals("load-cfg"))
        return CommandType_LoadCfg;
    else if (token.equals("test"))
    	return CommandType_Test;
    return CommandType_None;
}

struct Command readCommand(Stream &input)
{
	Command result;
    ParserState state = ParserState_Begin;
    String* this_arg_ptr = NULL;
    int args_to_read_left = 0;
    String token;
    token.reserve(16);
    while (state != ParserState_Done)
    {
    	const int this_char = input.peek();

    	switch (state)
		{
		case ParserState_Begin:
			if (this_char == TOKEN_COMMAND_DELIMITER)
			{
				state = ParserState_ParseCommandValue;
			}
			input.read();
			break;
		case ParserState_ParseCommandValue:
			if (this_char == TOKEN_COMMAND_DELIMITER ||
				this_char == TOKEN_COMMAND_ARGS_DELIMITER ||
				this_char <= 0)
			{
				result.m_type = parseCommandType(token);
				state = (this_char == TOKEN_COMMAND_ARGS_DELIMITER)
					? ParserState_ParseCommandArgsCountValue
					: ParserState_Done;
				if (this_char == TOKEN_COMMAND_ARGS_DELIMITER || this_char == 0)
				{
					input.read();
				}
				token.remove(0, token.length());
			}
			else
			{
				token += (char)input.read();
			}
			break;
		case ParserState_ParseCommandArgsCountValue:
			if (this_char == TOKEN_COMMAND_DELIMITER ||
				this_char == TOKEN_COMMAND_ARGS_DELIMITER ||
				this_char <= 0)
			{
				result.m_argc = args_to_read_left = token.toInt();
				if (result.m_argc > 0)
				{
					result.m_argv = new String[result.m_argc];
					this_arg_ptr = result.m_argv;
				}
				state = (this_char == TOKEN_COMMAND_ARGS_DELIMITER)
					? ParserState_ParseCommandArgValue
					: ParserState_Done;
				if (this_char == TOKEN_COMMAND_ARGS_DELIMITER || this_char == 0)
				{
					input.read();
				}
				token.remove(0, token.length());
			}
			else
			{
				token += (char)input.read();
			}
			break;
		case ParserState_ParseCommandArgValue:
			if (this_char == TOKEN_COMMAND_DELIMITER ||
				this_char == TOKEN_COMMAND_ARGS_DELIMITER ||
				this_char <= 0)
			{
				--args_to_read_left;
				if (result.m_argv != NULL && args_to_read_left >= 0)
				{
					*this_arg_ptr++ = token;
				}
				state = (this_char == TOKEN_COMMAND_ARGS_DELIMITER)
					? ParserState_ParseCommandArgValue
					: ParserState_Done;
				if (this_char == TOKEN_COMMAND_ARGS_DELIMITER || this_char == 0)
				{
					input.read();
				}
				token.remove(0, token.length());
			}
			else
			{
				token += (char)input.read();
			}
			break;
		default:
			Serial.println(token);
			state = ParserState_Done;
			break;
		}
    };

    if (result.m_type == CommandType_None ||
		(result.m_type != CommandType_None && args_to_read_left != 0))
    {
    	result.m_type = CommandType_None;
		if (result.m_argv != NULL)
		{
			delete [] result.m_argv;
			result.m_argv = NULL;
		}
		result.m_argc = -1;
    }

    return result;
}
