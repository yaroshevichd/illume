#line 2 "IllumeUtils_example.ino"
#include <Illume_CommandParsers.h>
#include <ArduinoUnit.h>


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

static CommandParser* g_CmdParser = getTextualCommandParser();

#if (1) // LoadCfgTests

struct LoadCfgIsParsedInTwoConsecutiveCalls : TestOnce
{
    LoadCfgIsParsedInTwoConsecutiveCalls()
        : TestOnce(F("load_cfg_is_parsed_in_two_consecutive_calls"))
    {}

    void once()
    {
        // INIT
        String testFirstPart(F("$load-"));
        String testSecondPart(F("cfg#"));
        StringStream firstStream(testFirstPart);
        StringStream secondStream(testSecondPart);

        // ACT
        assertEqual(g_CmdParser->parse(firstStream), ParserState_Continue);
        assertEqual(g_CmdParser->parse(secondStream), ParserState_Done);
        const Command* actualResult = g_CmdParser->command();

        // CHECK
        assertTrue(actualResult != NULL);
        assertEqual(actualResult->type, CommandType_LoadCfg);
        assertEqual(actualResult->argc, 0);
        assertTrue(actualResult->argv == NULL);
    }
} load_cfg_is_parsed_in_two_consecutive_calls_instance;

#endif // end LoadCfgTests


#if (1) // SaveCfgTests

static const byte SAVE_CFG_COMBINATIONS = 10;

struct SaveCfgTestPositiveTests : Test
{
    byte m_thisIndex;
    byte m_subTestsPass;
    SaveCfgCommand::SaveCfgParam m_expectedParam[SAVE_CFG_COMBINATIONS];

    SaveCfgTestPositiveTests(const __FlashStringHelper * test_name)
        : Test(test_name)
        , m_thisIndex(0)
        , m_subTestsPass(0)
    {
        for (byte i = 0; i < SAVE_CFG_COMBINATIONS; ++i)
        {
            switch (i)
            {
            case 0:
                m_expectedParam[i].info.animation = LedAnimation_On;
                m_expectedParam[i].duration = 11;
                break;
            case 1:
                m_expectedParam[i].info.animation = LedAnimation_Off;
                m_expectedParam[i].duration = 22;
                break;
            case 2:
                m_expectedParam[i].info.animation = LedAnimation_FadeIn;
                m_expectedParam[i].info.factor = LedAnimationFactor_Quarter;
                m_expectedParam[i].duration = 33;
                break;
            case 3:
                m_expectedParam[i].info.animation = LedAnimation_FadeIn;
                m_expectedParam[i].info.factor = LedAnimationFactor_Half;
                m_expectedParam[i].duration = 44;
                break;
            case 4:
                m_expectedParam[i].info.animation = LedAnimation_FadeIn;
                m_expectedParam[i].info.factor = LedAnimationFactor_Once;
                m_expectedParam[i].duration = 55;
                break;
            case 5:
                m_expectedParam[i].info.animation = LedAnimation_FadeIn;
                m_expectedParam[i].info.factor = LedAnimationFactor_Twice;
                m_expectedParam[i].duration = 66;
                break;
            case 6:
                m_expectedParam[i].info.animation = LedAnimation_FadeOut;
                m_expectedParam[i].info.factor = LedAnimationFactor_Quarter;
                m_expectedParam[i].duration = 77;
                break;
            case 7:
                m_expectedParam[i].info.animation = LedAnimation_FadeOut;
                m_expectedParam[i].info.factor = LedAnimationFactor_Half;
                m_expectedParam[i].duration = 88;
                break;
            case 8:
                m_expectedParam[i].info.animation = LedAnimation_FadeOut;
                m_expectedParam[i].info.factor = LedAnimationFactor_Once;
                m_expectedParam[i].duration = 99;
                break;
            case 9:
                m_expectedParam[i].info.animation = LedAnimation_FadeOut;
                m_expectedParam[i].info.factor = LedAnimationFactor_Twice;
                m_expectedParam[i].duration = 111;
                break;
            }
        }
    }

    virtual String getTestInput(const int index) const = 0;

    void loop()
    {
        if (m_thisIndex >= SAVE_CFG_COMBINATIONS)
        {
            m_subTestsPass == SAVE_CFG_COMBINATIONS
                ? pass()
                : fail();
            return;
        }
        ++m_thisIndex;

        // INIT
        String testInput = getTestInput(m_thisIndex-1);
        StringStream test_stream(testInput);

        // ACT
        assertEqual(g_CmdParser->parse(test_stream), ParserState_Done);
        const Command* actualResult = g_CmdParser->command();

        // CHECK
        assertTrue(actualResult != NULL);
        assertEqual(actualResult->type, CommandType_SaveCfg);
        assertEqual(actualResult->argc, 1);
        assertTrue(actualResult->argv != NULL);
        const SaveCfgCommand* save_cfg_cmd =
            reinterpret_cast<const SaveCfgCommand*>(actualResult);
        assertEqual(save_cfg_cmd->params()->info.name, m_expectedParam[m_thisIndex-1].info.name);
        assertEqual(save_cfg_cmd->params()->info.animation, m_expectedParam[m_thisIndex-1].info.animation);
        assertEqual(save_cfg_cmd->params()->duration, m_expectedParam[m_thisIndex-1].duration);
        assertEqual(save_cfg_cmd->params()->info.factor, m_expectedParam[m_thisIndex-1].info.factor);

        ++m_subTestsPass;
    }
};

#if (1) // begin SaveCfgTestParseIfGreenLedOccured

struct SaveCfgTestParseIfGreenLedOccured : SaveCfgTestPositiveTests
{
    SaveCfgTestParseIfGreenLedOccured()
        : SaveCfgTestPositiveTests(F("save_cfg_parsed_if_green_led_occured"))
    {
        for (byte i = 0; i < SAVE_CFG_COMBINATIONS; ++i)
        {
            m_expectedParam[i].info.name = LedName_Green;
        }
    }

    virtual String getTestInput(const int index) const
    {
        switch (index)
        {
        case 0:
            return String(F("$save-cfg,1,g:x:11#"));
        case 1:
            return String(F("$save-cfg,1,g:o:22#"));
        case 2:
            return String(F("$save-cfg,1,g:/:33:q#"));
        case 3:
            return String(F("$save-cfg,1,g:/:44:h#"));
        case 4:
            return String(F("$save-cfg,1,g:/:55:1#"));
        case 5:
            return String(F("$save-cfg,1,g:/:66:2#"));
        case 6:
            return String(F("$save-cfg,1,g:\\:77:q#"));
        case 7:
            return String(F("$save-cfg,1,g:\\:88:h#"));
        case 8:
            return String(F("$save-cfg,1,g:\\:99:1#"));
        case 9:
            return String(F("$save-cfg,1,g:\\:111:2#"));
        }
        return String();
    }
} save_cfg_parsed_if_green_led_occured_instance;

#endif // end SaveCfgTestParseIfGreenLedOccured

#if (1) // begin SaveCfgTestParseIfRedLedOccured

struct SaveCfgTestParseIfRedLedOccured : SaveCfgTestPositiveTests
{
    SaveCfgTestParseIfRedLedOccured()
        : SaveCfgTestPositiveTests(F("save_cfg_parsed_if_red_led_occured"))
    {
        for (byte i = 0; i < SAVE_CFG_COMBINATIONS; ++i)
        {
            m_expectedParam[i].info.name = LedName_Red;
        }
    }

    virtual String getTestInput(const int index) const
    {
        switch (index)
        {
        case 0:
            return String(F("$save-cfg,1,r:x:11#"));
        case 1:
            return String(F("$save-cfg,1,r:o:22#"));
        case 2:
            return String(F("$save-cfg,1,r:/:33:q#"));
        case 3:
            return String(F("$save-cfg,1,r:/:44:h#"));
        case 4:
            return String(F("$save-cfg,1,r:/:55:1#"));
        case 5:
            return String(F("$save-cfg,1,r:/:66:2#"));
        case 6:
            return String(F("$save-cfg,1,r:\\:77:q#"));
        case 7:
            return String(F("$save-cfg,1,r:\\:88:h#"));
        case 8:
            return String(F("$save-cfg,1,r:\\:99:1#"));
        case 9:
            return String(F("$save-cfg,1,r:\\:111:2#"));
        }
        return String();
    }
} save_cfg_parsed_if_red_led_occured_instance;

#endif // end SaveCfgTestParseIfRedLedOccured

#if (1) // begin SaveCfgTestParseIfBlueLedOccured

struct SaveCfgTestParseIfBlueLedOccured : SaveCfgTestPositiveTests
{
    SaveCfgTestParseIfBlueLedOccured()
        : SaveCfgTestPositiveTests(F("save_cfg_parsed_if_blue_led_occured"))
    {
        for (byte i = 0; i < SAVE_CFG_COMBINATIONS; ++i)
        {
            m_expectedParam[i].info.name = LedName_Blue;
        }
    }

    virtual String getTestInput(const int index) const
    {
        switch (index)
        {
        case 0:
            return String(F("$save-cfg,1,b:x:11#"));
        case 1:
            return String(F("$save-cfg,1,b:o:22#"));
        case 2:
            return String(F("$save-cfg,1,b:/:33:q#"));
        case 3:
            return String(F("$save-cfg,1,b:/:44:h#"));
        case 4:
            return String(F("$save-cfg,1,b:/:55:1#"));
        case 5:
            return String(F("$save-cfg,1,b:/:66:2#"));
        case 6:
            return String(F("$save-cfg,1,b:\\:77:q#"));
        case 7:
            return String(F("$save-cfg,1,b:\\:88:h#"));
        case 8:
            return String(F("$save-cfg,1,b:\\:99:1#"));
        case 9:
            return String(F("$save-cfg,1,b:\\:111:2#"));
        }
        return String();
    }
} save_cfg_parsed_if_blue_led_occured_instance;

#endif // end SaveCfgTestParseIfBlueLedOccured

#if (1) // begin SaveCfgTestParseIfYellowLedOccured

struct SaveCfgTestParseIfYellowLedOccured : SaveCfgTestPositiveTests
{
    SaveCfgTestParseIfYellowLedOccured()
        : SaveCfgTestPositiveTests(F("save_cfg_parsed_if_yellow_led_occured"))
    {
        for (byte i = 0; i < SAVE_CFG_COMBINATIONS; ++i)
        {
            m_expectedParam[i].info.name = LedName_Yellow;
        }
    }

    virtual String getTestInput(const int index) const
    {
        switch (index)
        {
        case 0:
            return String(F("$save-cfg,1,y:x:11#"));
        case 1:
            return String(F("$save-cfg,1,y:o:22#"));
        case 2:
            return String(F("$save-cfg,1,y:/:33:q#"));
        case 3:
            return String(F("$save-cfg,1,y:/:44:h#"));
        case 4:
            return String(F("$save-cfg,1,y:/:55:1#"));
        case 5:
            return String(F("$save-cfg,1,y:/:66:2#"));
        case 6:
            return String(F("$save-cfg,1,y:\\:77:q#"));
        case 7:
            return String(F("$save-cfg,1,y:\\:88:h#"));
        case 8:
            return String(F("$save-cfg,1,y:\\:99:1#"));
        case 9:
            return String(F("$save-cfg,1,y:\\:111:2#"));
        }
        return String();
    }
} save_cfg_parsed_if_yellow_led_occured_instance;

#endif // end SaveCfgTestParseIfYellowLedOccured

#if (1) // begin SaveCfgTestParseIfWhiteLedOccured

struct SaveCfgTestParseIfWhiteLedOccured : SaveCfgTestPositiveTests
{
    SaveCfgTestParseIfWhiteLedOccured()
        : SaveCfgTestPositiveTests(F("save_cfg_parsed_if_white_led_occured"))
    {
        for (byte i = 0; i < SAVE_CFG_COMBINATIONS; ++i)
        {
            m_expectedParam[i].info.name = LedName_White;
        }
    }

    virtual String getTestInput(const int index) const
    {
        switch (index)
        {
        case 0:
            return String(F("$save-cfg,1,w:x:11#"));
        case 1:
            return String(F("$save-cfg,1,w:o:22#"));
        case 2:
            return String(F("$save-cfg,1,w:/:33:q#"));
        case 3:
            return String(F("$save-cfg,1,w:/:44:h#"));
        case 4:
            return String(F("$save-cfg,1,w:/:55:1#"));
        case 5:
            return String(F("$save-cfg,1,w:/:66:2#"));
        case 6:
            return String(F("$save-cfg,1,w:\\:77:q#"));
        case 7:
            return String(F("$save-cfg,1,w:\\:88:h#"));
        case 8:
            return String(F("$save-cfg,1,w:\\:99:1#"));
        case 9:
            return String(F("$save-cfg,1,w:\\:111:2#"));
        }
        return String();
    }
} save_cfg_parsed_if_white_led_occured_instance;

#endif // end SaveCfgTestParseIfWhiteLedOccured

#endif // end SaveCfgTests

void setup()
{
    Serial.begin(9600);
    while(!Serial);
}

void loop()
{
    Test::run();
}
