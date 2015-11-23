//#define _DEBUG_CMD_PARSER
#include <Arduino.h>
#include "IllumeUtils.h"
#include <stdlib.h>

#define TOKEN_COMMAND_DELIMITER '$'
#define TOKEN_COMMAND_END_DELIMITER '#'
#define TOKEN_COMMAND_ARGS_DELIMITER ','
#define TOKEN_COMMAND_ARG_FIELD_DELIMITER ':'


LedName parseLedName(const String& token)
{
#ifdef _DEBUG_CMD_PARSER
    Serial.print("CmdParser: parseLedName(");
    Serial.print(token);
    Serial.println(")");
#endif
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
#ifdef _DEBUG_CMD_PARSER
    Serial.print("CmdParser: parseLedEffect(");
    Serial.print(token);
    Serial.println(")");
#endif
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
#ifdef _DEBUG_CMD_PARSER
    Serial.print("CmdParser: parseCommandType(");
    Serial.print(token);
    Serial.println(")");
#endif
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
#ifdef _DEBUG_CMD_PARSER
        Serial.println("CmdParser::reset");
#endif
        resetState();
        resetToken();
        resetCommand();
        resetIndexes();
    }

    virtual ParserState parse(Stream &input)
    {
#ifdef _DEBUG_CMD_PARSER
        Serial.println("CmdParser::parse");
#endif
        if (state == ParserState_Done)
        {
            reset();
        }

        bool moreDataRequired = false;
        while (state != ParserState_Done && !moreDataRequired)
        {
            const int this_char = input.peek();
//            Serial.print((char)this_char);

            switch (state)
            {
            case ParserState_Begin:
                switch (this_char)
                {
                case TOKEN_COMMAND_DELIMITER:
                    switchState(ParserState_ParseCommandValue);
                    resetIndexes();
                    break;
                case 0:
                case -1:
                    moreDataRequired = true;
                    break;
                default:
                    break;
                }
                skip(input);
                break;
            case ParserState_ParseCommandValue:
                switch (this_char)
                {
                case TOKEN_COMMAND_END_DELIMITER:
                case TOKEN_COMMAND_ARGS_DELIMITER:
                    cmdType = parseCommandType(token);
                    switch (cmdType)
                    {
                    case CommandType_SaveCfg:
                        switchState(ParserState_ParseCommandArgsCountValue);
                        break;
                    default:
                        switchState(ParserState_Done);
                        break;
                    }
                    resetToken();
                    skip(input);
                    break;
                case 0:
                case -1:
                    moreDataRequired = true;
                    skip(input);
                    break;
                default:
                    consume(input);
                    break;
                }
                break;
            case ParserState_ParseCommandArgsCountValue:
                switch (this_char)
                {
                case TOKEN_COMMAND_END_DELIMITER:
                    switchState(ParserState_Done);
                    resetToken();
                    break;
                case TOKEN_COMMAND_ARGS_DELIMITER:
                    fieldIndex = 0;
                    argsLeft = cmdArgs = token.toInt();
                    cmdParams = instanceParamsByType(cmdType, cmdArgs);
                    switchState(ParserState_ParseCommandArgValue);
                    resetToken();
                    skip(input);
                    break;
                case 0:
                case -1:
                    moreDataRequired = true;
                    skip(input);
                    break;
                default:
                    consume(input);
                    break;
                }
                break;
            case ParserState_ParseCommandArgValue:
                switch (this_char)
                {
                case TOKEN_COMMAND_END_DELIMITER:
                    switchState(ParserState_Done);
                    parseCommandArg();
                    resetToken();
                    break;
                case TOKEN_COMMAND_ARGS_DELIMITER:
                    if (argsLeft > 0)
                    {
                        parseCommandArg();
                        fieldIndex = 0;
                    }
                    --argsLeft;
                    resetToken();
                    skip(input);
                    break;
                case TOKEN_COMMAND_ARG_FIELD_DELIMITER:
                    parseCommandArg();
                    ++fieldIndex;
                    resetToken();
                    skip(input);
                    break;
                case 0:
                case -1:
                    moreDataRequired = true;
                    skip(input);
                    break;
                default:
                    consume(input);
                    break;
                }
                break;
            default:
                break;
            }
        };

        return moreDataRequired ? ParserState_Continue : state;
    }

    virtual const Command* command()
    {
#ifdef _DEBUG_CMD_PARSER
        Serial.println("CmdParser::command");
#endif
        if (state == ParserState_Done && cmdParsed == NULL)
        {
            switch (cmdType)
            {
            case CommandType_SaveCfg:
                cmdParsed = new SaveCfgCommand(cmdArgs, cmdParams);
                break;
            case CommandType_LoadCfg:
                cmdParsed = new LoadCfgCommand();
                break;
            default:
                break;
            }
        }
        return cmdParsed;
    }

private:
    void resetToken()
    {
        token.remove(0, token.length());
    }

    void resetIndexes()
    {
        argsLeft = fieldIndex = 0;
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

    void skip(Stream& input)
    {
        input.read();
    }

    void switchState(const ParserState new_state)
    {
#ifdef _DEBUG_CMD_PARSER
        Serial.print("CmdParser: ");
        Serial.print(state);
        Serial.print(" -> ");
        Serial.println(new_state);
#endif
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

    void parseCommandArg()
    {
        const int index = cmdArgs - argsLeft;
        switch (cmdType)
        {
        case CommandType_SaveCfg:
            switch (fieldIndex)
            {
            case 0:
                reinterpret_cast<SaveCfgCommand::SaveCfgParam*>(cmdParams)[index].name = parseLedName(token);
                break;
            case 1:
                reinterpret_cast<SaveCfgCommand::SaveCfgParam*>(cmdParams)[index].effect = parseLedEffect(token);
                break;
            case 2:
                reinterpret_cast<SaveCfgCommand::SaveCfgParam*>(cmdParams)[index].ticks = token.toInt();
                break;
            case 3:
                reinterpret_cast<SaveCfgCommand::SaveCfgParam*>(cmdParams)[index].extra[0] = token[0];
                if (token.length() > 1)
                {
                    reinterpret_cast<SaveCfgCommand::SaveCfgParam*>(cmdParams)[index].extra[1] = token[1];
                }
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

} g_CommandParserImpl;

CommandParser* g_CmdParser = &g_CommandParserImpl;
