//#define _ENABLE_TEXTUAL_COMMAND_PARSER_DEBUG
#include <Illume_CommandParsers.h>
#include <Arduino.h>
#include <stdlib.h>

#define TOKEN_COMMAND_DELIMITER '$'
#define TOKEN_COMMAND_END_DELIMITER '#'
#define TOKEN_COMMAND_ARGS_DELIMITER ','
#define TOKEN_COMMAND_ARG_FIELD_DELIMITER ':'


bool parseLedName(const String& token, LedName& result)
{
#ifdef _ENABLE_TEXTUAL_COMMAND_PARSER_DEBUG
    Serial.print(F("TextualCommandParser: parseLedName("));
    Serial.print(token);
    Serial.println(F(")"));
#endif
    if (token.length() == 1)
    {
        switch (token[0])
        {
        case 'r':
            result = LedName_Red;
            return true;
        case 'g':
            result = LedName_Green;
            return true;
        case 'b':
            result = LedName_Blue;
            return true;
        case 'y':
            result = LedName_Yellow;
            return true;
        case 'w':
            result = LedName_White;
            return true;
        case '*':
            result = LedName_Any;
            return true;
        }
    }
    return false;
}

bool parseLedAnimation(const String& token, LedAnimation& result)
{
#ifdef _ENABLE_TEXTUAL_COMMAND_PARSER_DEBUG
    Serial.print(F("TextualCommandParser: parseLedEffect("));
    Serial.print(token);
    Serial.println(F(")"));
#endif
    if (token.length() == 1)
    {
        switch (token[0])
        {
        case 'x':
            result = LedAnimation_On;
            return true;
        case 'o':
            result = LedAnimation_Off;
            return true;
        case '/':
            result = LedAnimation_FadeIn;
            return true;
        case '\\':
            result = LedAnimation_FadeOut;
            return true;
        }
    }
    return false;
}

bool parseLedAnimationFactor(const String& token, LedAnimationFactor& result)
{
#ifdef _ENABLE_TEXTUAL_COMMAND_PARSER_DEBUG
    Serial.print(F("TextualCommandParser: parseLedAnimationFactor("));
    Serial.print(token);
    Serial.println(F(")"));
#endif
    if (token.length() == 1)
    {
        switch (token[0])
        {
        case 'q':
            result = LedAnimationFactor_Quarter;
            return true;
        case 'h':
            result = LedAnimationFactor_Half;
            return true;
        case '1':
            result = LedAnimationFactor_Once;
            return true;
        case '2':
            result = LedAnimationFactor_Twice;
            return true;
        }
    }
    return false;
}

bool parseBool(const String& token, bool& result)
{
#ifdef _ENABLE_TEXTUAL_COMMAND_PARSER_DEBUG
    Serial.print(F("TextualCommandParser: parseBool("));
    Serial.print(token);
    Serial.println(F(")"));
#endif
    if (token.length() == 1)
    {
        switch (token[0])
        {
        case '1':
            result = true;
            return true;
        case '0':
            result = false;
            return true;
        }
    }
    return false;
}

bool parseCommandType(const String& token, CommandType& result)
{
#ifdef _ENABLE_TEXTUAL_COMMAND_PARSER_DEBUG
    Serial.print(F("TextualCommandParser: parseCommandType("));
    Serial.print(token);
    Serial.println(F(")"));
#endif
    if (token.equals("save-cfg"))
    {
        result = CommandType_SaveCfg;
        return true;
    }
    else if (token.equals("load-cfg"))
    {
        result = CommandType_LoadCfg;
        return true;
    }
    return false;
}


class TextualCommandParser : public CommandParser
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
    TextualCommandParser()
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
#ifdef _ENABLE_TEXTUAL_COMMAND_PARSER_DEBUG
        Serial.println(F("TextualCommandParser::reset"));
#endif
        resetState();
        resetToken();
        resetCommand();
        resetIndexes();
    }

    virtual ParserState parse(Stream &input)
    {
#ifdef _ENABLE_TEXTUAL_COMMAND_PARSER_DEBUG
        Serial.println(F("TextualCommandParser::parse"));
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
#ifdef _ENABLE_TEXTUAL_COMMAND_PARSER_DEBUG
                    Serial.println(F("TextualCommandParser::parse: EOL/EOS occurred!"));
#endif
                    moreDataRequired = true;
                    skip(input);
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
                    parseCommandType(token, cmdType);
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
#ifdef _ENABLE_TEXTUAL_COMMAND_PARSER_DEBUG
                    Serial.println(F("TextualCommandParser::parse: EOL/EOS occurred!"));
#endif
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
#ifdef _ENABLE_TEXTUAL_COMMAND_PARSER_DEBUG
                    Serial.println(F("TextualCommandParser::parse: EOL/EOS occurred!"));
#endif
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
#ifdef _ENABLE_TEXTUAL_COMMAND_PARSER_DEBUG
                    Serial.println(F("TextualCommandParser::parse: EOL/EOS occurred!"));
#endif
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
#ifdef _ENABLE_TEXTUAL_COMMAND_PARSER_DEBUG
        Serial.println(F("TextualCommandParser::command"));
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
#ifdef _ENABLE_TEXTUAL_COMMAND_PARSER_DEBUG
        Serial.print(F("TextualCommandParser: "));
        Serial.print(state);
        Serial.print(F(" -> "));
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
            {
                SaveCfgCommand::SaveCfgParam* params = reinterpret_cast<SaveCfgCommand::SaveCfgParam*>(cmdParams);
                switch (fieldIndex)
                {
                case 0:
                    LedName ledName;
                    if (parseLedName(token, ledName))
                    {
                        params[index].info.name = ledName;
                    }
                    break;
                case 1:
                    LedAnimation ledAnimation;
                    if (parseLedAnimation(token, ledAnimation))
                    {
                        params[index].info.animation = ledAnimation;
                    }
                    break;
                case 2:
                    params[index].duration = token.toInt();
                    break;
                case 3:
                    LedAnimationFactor ledAnimationFactor;
                    if (parseLedAnimationFactor(token, ledAnimationFactor))
                    {
                        params[index].info.factor = ledAnimationFactor;
                    }
                    break;
                case 4:
                    bool ledStepOnce;
                    if (parseBool(token, ledStepOnce))
                    {
                        params[index].info.once = ledStepOnce;
                    }
                    break;
                default:
                    break;
                }
                break;
            }
        default:
            break;
        }
    }

} g_TextualCommandParser;

CommandParser* getTextualCommandParser()
{
    return &g_TextualCommandParser;
}
