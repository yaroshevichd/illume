#include <Arduino.h>
#include "IllumeUtils.h"
#include <stdlib.h>

#define TOKEN_COMMAND_DELIMITER '$'
#define TOKEN_COMMAND_ARGS_DELIMITER ','
#define TOKEN_COMMAND_ARG_FIELD_DELIMITER ':'


struct _CurrentParserState
{
	String token;
	int argsLeft;
	int argIndex;
	int fieldIndex;
	ParserState state;

	_CurrentParserState()
		: token()
		, argsLeft(0)
		, argIndex(0)
		, fieldIndex(0)
		, state(ParserState_Begin)
	{
		token.reserve(16);
	}

	void resetToken()
	{
		token.remove(0, token.length());
	}

	void resetIndexes()
	{
		argsLeft = argIndex = fieldIndex = 0;
	}
} g_ParserState;

static struct Command g_Command;


CommandType parseCommandType(const String& token)
{
    if (token.equals("save-cfg"))
        return CommandType_SaveCfg;
    else if (token.equals("load-cfg"))
        return CommandType_LoadCfg;
    return CommandType_None;
}

Command::CommandParam* instanceParamsByType(const CommandType &type, const int argc)
{
    switch (type)
    {
    case CommandType_SaveCfg:
        return new SaveCfgCommand::SaveCfgParam[argc];
    default:
        return NULL;
    }
}

LedName parseLedName(const String& token)
{
	if (token.length() == 1)
	{
		switch (token[0])
		{
		case 'r':
			return LedName_Red;
		case 'g':
			return LedName_Green;
		case 'b':
			return LedName_Blue;
		case 'y':
			return LedName_Yellow;
		case 'w':
			return LedName_White;
		}
	}
   return LedName_None;
}

LedEffect parseLedEffect(const String& token)
{
	if (token.length() == 1)
	{
		switch (token[0])
		{
		case 'x':
			return LedEffect_On;
		case 'o':
			return LedEffect_Off;
		case '/':
			 return LedEffect_FadeIn;
		case '\\':
			return LedEffect_FadeOut;
		}
	}
    return LedEffect_None;
}

void resetParser()
{
	g_Command.argc = -1;
	g_Command.type = CommandType_None;
	if (g_Command.argv != NULL)
	{
		delete g_Command.argv;
		g_Command.argv = NULL
	}
	g_ParserState.state = ParserState_Begin;
	g_ParserState.resetToken();
	g_ParserState.resetIndexes();
}

ParserState parseInput(Stream &input)
{
    CommandType command_type = CommandType_None;
    Command::CommandParam* command_params = NULL;
    while (g_ParserState.state != ParserState_Done)
    {
        const int this_char = input.peek();
//        Serial.print((char)this_char);

        switch (g_ParserState.state)
        {
        case ParserState_Begin:
            if (this_char == TOKEN_COMMAND_DELIMITER)
            {
            	g_ParserState.state = ParserState_ParseCommandValue;
            	g_ParserState.resetIndexes();
            }
            input.read();
            break;
        case ParserState_ParseCommandValue:
            if (this_char == TOKEN_COMMAND_DELIMITER ||
                this_char == TOKEN_COMMAND_ARGS_DELIMITER ||
                this_char <= 0)
            {
                command_type = parseCommandType(token);
                g_ParserState.state = (this_char == TOKEN_COMMAND_ARGS_DELIMITER)
                    ? (command_type == CommandType_SaveCfg
                        ? ParserState_ParseCommandArgsCountValue
                        : ParserState_ParseCommandArgValue)
                    : ParserState_Done;
                if (this_char == TOKEN_COMMAND_ARGS_DELIMITER || this_char == 0)
                {
                    input.read();
                }
                g_ParserState.resetToken();
            }
            else
            {
                g_Token += (char)input.read();
            }
            break;
        case ParserState_ParseCommandArgsCountValue:
            if (this_char == TOKEN_COMMAND_DELIMITER ||
                this_char == TOKEN_COMMAND_ARGS_DELIMITER ||
                this_char <= 0)
            {
                command_args = args_to_read_left = token.toInt();
                command_params = instanceParamsByType(command_type, command_args);
                g_ParserState.state = (this_char == TOKEN_COMMAND_ARGS_DELIMITER)
                    ? ParserState_ParseCommandArgValue
                    : ParserState_Done;
                if (this_char == TOKEN_COMMAND_ARGS_DELIMITER || this_char == 0)
                {
                    input.read();
                }
                g_ParserState.resetToken();
            }
            else
            {
            	g_Token += (char)input.read();
            }
            break;
        case ParserState_ParseCommandArgValue:
            if (this_char == TOKEN_COMMAND_DELIMITER ||
                this_char == TOKEN_COMMAND_ARGS_DELIMITER ||
                this_char == TOKEN_COMMAND_ARG_FIELD_DELIMITER ||
                this_char <= 0)
            {
                if (this_char != TOKEN_COMMAND_ARG_FIELD_DELIMITER)
                {
                    --args_to_read_left;
                }
                if (command_params != NULL && args_to_read_left >= 0)
                {
                    switch (command_type)
                    {
                    case CommandType_SaveCfg:
                        switch (this_arg_field_idx)
                        {
                        case 0:
                            reinterpret_cast<SaveCfgCommand::SaveCfgParam*>(command_params)[this_arg_idx].name = parseLedName(token);
                            break;
                        case 1:
                            reinterpret_cast<SaveCfgCommand::SaveCfgParam*>(command_params)[this_arg_idx].effect = parseLedEffect(token);
                            break;
                        case 2:
                            reinterpret_cast<SaveCfgCommand::SaveCfgParam*>(command_params)[this_arg_idx].ticks = token.toInt();
                            break;
                        case 3:
                            reinterpret_cast<SaveCfgCommand::SaveCfgParam*>(command_params)[this_arg_idx].extra[0] = token[0];
                            if (token.length() > 1)
                            {
                                reinterpret_cast<SaveCfgCommand::SaveCfgParam*>(command_params)[this_arg_idx].extra[1] = token[1];
                            }
                            break;
                        default:
                            break;
                        }
                        ++this_arg_field_idx;
                        break;
                    default:
                        break;
                    }
                }
                if (this_char == TOKEN_COMMAND_ARGS_DELIMITER)
                {
                    this_arg_field_idx = 0;
                    ++this_arg_idx;
                }
                g_ParserState.state = (this_char == TOKEN_COMMAND_ARGS_DELIMITER || this_char == TOKEN_COMMAND_ARG_FIELD_DELIMITER)
                    ? ParserState_ParseCommandArgValue
                    : ParserState_Done;
                if (this_char == TOKEN_COMMAND_ARGS_DELIMITER || this_char == TOKEN_COMMAND_ARG_FIELD_DELIMITER || this_char == 0)
                {
                    input.read();
                }
                g_ParserState.resetToken();
            }
            else
            {
            	g_Token += (char)input.read();
            }
            break;
        default:
//            Serial.println(token);
            g_ParserState.state = ParserState_Done;
            break;
        }
    };

//    if (command_type == CommandType_None ||
//        (command_type != CommandType_None && args_to_read_left != 0))
//    {
//        if (command_params != NULL)
//        {
//            delete [] command_params;
//        }
//        return NULL;
//    }
//
//    Command* result = NULL;
//    switch (command_type)
//    {
//    case CommandType_SaveCfg:
//        result = new SaveCfgCommand(command_args, command_params);
//        break;
//    default:
//        break;
//    }
//    Serial.println((int)result);
    return g_ParserState;
}
