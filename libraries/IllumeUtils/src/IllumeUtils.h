#ifndef __ILLUME_UTILS_H__
#define __ILLUME_UTILS_H__

#include <Stream.h>

enum CommandType
{
    CommandType_SaveCfg,
    CommandType_LoadCfg,
    CommandType_None
};

enum LedName
{
    LedName_None = 0,
    LedName_Red = 'r',
    LedName_Green = 'g',
    LedName_Blue = 'b',
    LedName_Yellow = 'y',
    LedName_White = 'w'
};

enum LedEffect
{
    LedEffect_None = 0,
    LedEffect_On = 'x',
    LedEffect_Off = 'o',
    LedEffect_FadeIn = '/',
    LedEffect_FadeOut = '\\'
};

enum LedFadeType
{
    LedFadeType_Linear = '-',
    LedFadeType_Exponential = 'e'
};

struct Command
{
    struct CommandParam
    {
    };

    CommandType type;
    int argc;
    CommandParam* argv;

    Command(const CommandType type, const int argc, CommandParam* const argv)
        : type(type)
        , argc(argc)
        , argv(argv)
    {}

//    virtual ~Command()
//    {
//        Serial.println("~Command");
//        delete [] argv;
//    }
};

struct SaveCfgCommand : Command
{
    struct SaveCfgParam : public CommandParam
    {
        LedName name;
        LedEffect effect;
        int ticks;
        char extra[2];

        SaveCfgParam()
            : name(LedName_None)
            , effect(LedEffect_None)
            , ticks(0)
        {
            extra[0] = extra[1] = 0;
        }
    };

    SaveCfgCommand(const int argc, CommandParam* const argv)
        : Command(CommandType_SaveCfg, argc, argv)
    {}

    SaveCfgParam* params() const
    {
        return reinterpret_cast<SaveCfgParam*>(argv);
    }
};

struct LoadCfgCommand : Command
{
    LoadCfgCommand()
        : Command(CommandType_LoadCfg, -1, NULL)
    {}
};


enum ParserState
{
    ParserState_Begin,
    ParserState_ParseCommandType,
    ParserState_ParseCommandValue,
    ParserState_ParseCommandArgValue,
    ParserState_ParseCommandArgsCountValue,
    ParserState_Continue,
    ParserState_Done
};

class CommandParser
{
public:
    virtual ~CommandParser() {}
    virtual void reset() = 0;
    virtual ParserState parse(Stream &input) = 0;
    virtual const Command* command() const = 0;
};

extern CommandParser* g_CmdParser;


class StringStream : public Stream
{
private:
    const String &string;
    unsigned position;

public:
    StringStream(const String &s)
        : string(s)
        , position(0)
    {
    }

    virtual ~StringStream()
    {}

    // Stream methods
    virtual int available()
    {
        return string.length() - position;
    }
    
    virtual int read()
    {
        return position < string.length() ? string[position++] : -1;
    }
    
    virtual int peek()
    {
        return position < string.length() ? string[position] : -1;
    }
    
    virtual void flush()
    {};
    
    // Print methods
    virtual size_t write(uint8_t /*c*/)
    {
        return 0;
    };
};

#endif // __ILLUME_UTILS_H__
