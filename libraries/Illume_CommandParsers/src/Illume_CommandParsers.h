#ifndef __ILLUME_UTILS_H__
#define __ILLUME_UTILS_H__

#include <Illume_LedProcessor.h>
#include <Stream.h>

enum CommandType
{
    CommandType_SaveCfg,
    CommandType_LoadCfg,
    CommandType_None
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
};

struct SaveCfgCommand : Command
{
    struct SaveCfgParam : public CommandParam, public LedAnimationStep
    {
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
        : Command(CommandType_LoadCfg, 0, NULL)
    {}
};


enum ParserState
{
    ParserState_Begin,
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
    virtual const Command* command() = 0;
};

CommandParser* getTextualCommandParser();

#endif // __ILLUME_UTILS_H__
