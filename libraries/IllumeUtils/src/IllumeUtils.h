#ifndef __ILLUME_UTILS_H__
#define __ILLUME_UTILS_H__

#include <Stream.h>

enum CommandType
{
    CommandType_SaveCfg,
    CommandType_LoadCfg,
    CommandType_Test,
    CommandType_None
};

struct Command
{
	CommandType m_type;
	int m_argc;
	String* m_argv;

    Command()
        : m_type(CommandType_None)
		, m_argc(-1)
		, m_argv(NULL)
    {}
};

struct Command readCommand(Stream &input);


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
