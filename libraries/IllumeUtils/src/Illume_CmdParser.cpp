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
    if (token.equals("r"))
        return LedName_Red;
    else if (token.equals("g"))
        return LedName_Green;
    else if (token.equals("b"))
        return LedName_Blue;
    else if (token.equals("y"))
        return LedName_Yellow;
    else if (token.equals("w"))
        return LedName_White;
    return LedName_None;
}

LedEffect parseLedEffect(const String& token)
{
    if (token.equals("x"))
        return LedEffect_On;
    else if (token.equals("o"))
        return LedEffect_Off;
    else if (token.equals("/"))
        return LedEffect_FadeIn;
    else if (token.equals("\\"))
        return LedEffect_FadeOut;
    return LedEffect_None;
}

Command* readCommand(Stream &input)
{
    ParserState state = ParserState_Begin;
    CommandType command_type = CommandType_None;
    Command::CommandParam* command_params = NULL;
    int command_args = 0, args_to_read_left = 0, this_arg_idx = 0, this_arg_field_idx = 0;
    String token;
    token.reserve(16);
    while (state != ParserState_Done)
    {
        const int this_char = input.peek();
//        Serial.print((char)this_char);

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
                command_type = parseCommandType(token);
                state = (this_char == TOKEN_COMMAND_ARGS_DELIMITER)
                    ? (command_type == CommandType_SaveCfg
                        ? ParserState_ParseCommandArgsCountValue
                        : ParserState_ParseCommandArgValue)
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
                command_args = args_to_read_left = token.toInt();
                command_params = instanceParamsByType(command_type, command_args);
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
                state = (this_char == TOKEN_COMMAND_ARGS_DELIMITER || this_char == TOKEN_COMMAND_ARG_FIELD_DELIMITER)
                    ? ParserState_ParseCommandArgValue
                    : ParserState_Done;
                if (this_char == TOKEN_COMMAND_ARGS_DELIMITER || this_char == TOKEN_COMMAND_ARG_FIELD_DELIMITER || this_char == 0)
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

    if (command_type == CommandType_None ||
        (command_type != CommandType_None && args_to_read_left != 0))
    {
        if (command_params != NULL)
        {
            delete [] command_params;
        }
        return NULL;
    }

    Command* result = NULL;
    switch (command_type)
    {
    case CommandType_SaveCfg:
        result = new SaveCfgCommand(command_args, command_params);
        break;
    default:
        break;
    }
//    Serial.println((int)result);
    return result;
}
