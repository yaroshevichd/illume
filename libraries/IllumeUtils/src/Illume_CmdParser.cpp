#include <Arduino.h>
#include "IllumeUtils.h"
#include <stdlib.h>

#define TOKEN_COMMAND_DELIMITER '$'
#define TOKEN_COMMAND_ARGS_DELIMITER ','
#define TOKEN_COMMAND_ARG_FIELD_DELIMITER ':'


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

CommandType parseCommandType(const String& token)
{
    if (token.equals("save-cfg"))
    {
        return CommandType_SaveCfg;
    }
    else if (token.equals("load-cfg"))
    {
        return CommandType_LoadCfg;
    }
    return CommandType_None;
}


class CommandParserImpl : public CommandParser
{
    String token;
    int argsLeft;
    int argIndex;
    int fieldIndex;
    ParserState state;

    int cmdArgs;
    CommandType cmdType;
    Command::CommandParam* cmdParams;

    const Command * cmdParsed;

public:
    CommandParserImpl()
        : token()
        , argsLeft(0)
        , argIndex(0)
        , fieldIndex(0)
        , state(ParserState_Begin)
        , cmdArgs(-1)
        , cmdType(CommandType_None)
        , cmdParams(NULL)
        , cmdParsed(NULL)
    {
        token.reserve(16);
    }

    virtual void reset()
    {
        resetState();
        resetToken();
        resetCommand();
        resetIndexes();
    }

    virtual ParserState parse(Stream &input)
    {
        if (state == ParserState_Done)
        {
            reset();
        }

        while (state != ParserState_Done && state != ParserState_Continue)
        {
            const int this_char = input.peek();
//            Serial.print((char)this_char);

            switch (state)
            {
            case ParserState_Begin:
                if (this_char == TOKEN_COMMAND_DELIMITER)
                {
                    state = ParserState_ParseCommandValue;
                    resetIndexes();
                }
                input.read();
                break;
            case ParserState_ParseCommandValue:
                if (this_char == TOKEN_COMMAND_DELIMITER ||
                    this_char == TOKEN_COMMAND_ARGS_DELIMITER ||
                    this_char <= 0)
                {
                    cmdType = parseCommandType(token);
                    switchState((this_char == TOKEN_COMMAND_ARGS_DELIMITER)
                        ? (cmdType == CommandType_SaveCfg
                            ? ParserState_ParseCommandArgsCountValue
                            : ParserState_ParseCommandArgValue)
                        : ParserState_Done);
                    if (this_char == TOKEN_COMMAND_ARGS_DELIMITER || this_char == 0)
                    {
                        input.read();
                    }
                    resetToken();
                }
                else
                {
                    consume(input);
                }
                break;
            case ParserState_ParseCommandArgsCountValue:
                if (this_char == TOKEN_COMMAND_DELIMITER ||
                    this_char == TOKEN_COMMAND_ARGS_DELIMITER ||
                    this_char <= 0)
                {
                    argsLeft = cmdArgs = token.toInt();
                    cmdParams = instanceParamsByType(cmdType, cmdArgs);
                    switchState((this_char == TOKEN_COMMAND_ARGS_DELIMITER)
                        ? ParserState_ParseCommandArgValue
                        : ParserState_Done);
                    if (this_char == TOKEN_COMMAND_ARGS_DELIMITER || this_char == 0)
                    {
                        input.read();
                    }
                    resetToken();
                }
                else
                {
                    consume(input);
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
                        --argsLeft;
                    }
                    if (cmdParams != NULL && argsLeft >= 0)
                    {
                        switch (cmdType)
                        {
                        case CommandType_SaveCfg:
                            switch (fieldIndex)
                            {
                            case 0:
                                reinterpret_cast<SaveCfgCommand::SaveCfgParam*>(cmdParams)[argIndex].name = parseLedName(token);
                                break;
                            case 1:
                                reinterpret_cast<SaveCfgCommand::SaveCfgParam*>(cmdParams)[argIndex].effect = parseLedEffect(token);
                                break;
                            case 2:
                                reinterpret_cast<SaveCfgCommand::SaveCfgParam*>(cmdParams)[argIndex].ticks = token.toInt();
                                break;
                            case 3:
                                reinterpret_cast<SaveCfgCommand::SaveCfgParam*>(cmdParams)[argIndex].extra[0] = token[0];
                                if (token.length() > 1)
                                {
                                    reinterpret_cast<SaveCfgCommand::SaveCfgParam*>(cmdParams)[argIndex].extra[1] = token[1];
                                }
                                break;
                            default:
                                break;
                            }
                            ++fieldIndex;
                            break;
                        default:
                            break;
                        }
                    }
                    if (this_char == TOKEN_COMMAND_ARGS_DELIMITER)
                    {
                        fieldIndex = 0;
                        ++argIndex;
                    }
                    switchState((this_char == TOKEN_COMMAND_ARGS_DELIMITER || this_char == TOKEN_COMMAND_ARG_FIELD_DELIMITER)
                        ? ParserState_ParseCommandArgValue
                        : ParserState_Done);
                    if (this_char == TOKEN_COMMAND_ARGS_DELIMITER || this_char == TOKEN_COMMAND_ARG_FIELD_DELIMITER || this_char == 0)
                    {
                        input.read();
                    }
                    resetToken();
                }
                else
                {
                    consume(input);
                }
                break;
            default:
                Serial.println(token);
                if (this_char >= 0)
                {
                    input.read();
                }
                switchState(ParserState_Continue);
                break;
            }
        };

        if (state == ParserState_Done)
        {
            switch (cmdType)
            {
            case CommandType_SaveCfg:
                cmdParsed = new SaveCfgCommand(cmdArgs, cmdParams);
                break;
            default:
                break;
            }
        }
        return state;
    }

    virtual const Command* command() const
    {
        return cmdParsed;
    }

private:
    void resetToken()
    {
        token.remove(0, token.length());
    }

    void resetIndexes()
    {
        argsLeft = argIndex = fieldIndex = 0;
    }

    void resetCommand()
    {
        cmdArgs = -1;
        cmdType = CommandType_None;
        if (cmdParams != NULL)
        {
            delete [] cmdParams;
            cmdParams = NULL;
        }
        if (cmdParsed != NULL)
        {
            delete cmdParsed;
            cmdParsed = NULL;
        }
    }

    void resetState()
    {
        switchState(ParserState_Begin);
    }

    void consume(Stream& input)
    {
        token += (char)input.read();
    }

    void switchState(const ParserState new_state)
    {
//        Serial.print(state);
//        Serial.print(" -> ");
//        Serial.println(new_state);
        state = new_state;
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

} g_CommandParserImpl;

CommandParser* g_CmdParser = &g_CommandParserImpl;
