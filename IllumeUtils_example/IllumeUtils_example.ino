#line 2 "IllumeUtils_example.ino"
#include <ArduinoUnit.h>
#include <IllumeUtils.h>


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
        const Command* actual_result = g_CmdParser->command();

        // CHECK
        assertTrue(actual_result != NULL);
        assertEqual(actual_result->type, CommandType_LoadCfg);
        assertEqual(actual_result->argc, 0);
        assertTrue(actual_result->argv == NULL);
    }
} load_cfg_is_parsed_in_two_consecutive_calls_instance;

#endif // end LoadCfgTests


#if (1) // SaveCfgTests

static const byte SAVE_CFG_COMBINATIONS = 6;

struct SaveCfgTestPositiveTests : Test
{
    byte m_thisIndex;
    byte m_subTestsPass;
    String m_testInput[SAVE_CFG_COMBINATIONS];
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
                m_expectedParam[i].effect = LedEffect_On;
                m_expectedParam[i].ticks = 11;
                break;
            case 1:
                m_expectedParam[i].effect = LedEffect_Off;
                m_expectedParam[i].ticks = 22;
                break;
            case 2:
                m_expectedParam[i].effect = LedEffect_FadeIn;
                m_expectedParam[i].extra[0] = LedFadeType_Linear;
                m_expectedParam[i].ticks = 33;
                break;
            case 3:
                m_expectedParam[i].effect = LedEffect_FadeIn;
                m_expectedParam[i].extra[0] = LedFadeType_Exponential;
                m_expectedParam[i].ticks = 44;
                break;
            case 4:
                m_expectedParam[i].effect = LedEffect_FadeOut;
                m_expectedParam[i].extra[0] = LedFadeType_Linear;
                m_expectedParam[i].ticks = 55;
                break;
            case 5:
                m_expectedParam[i].effect = LedEffect_FadeOut;
                m_expectedParam[i].extra[0] = LedFadeType_Exponential;
                m_expectedParam[i].ticks = 66;
                break;
            }
        }
    }

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
        StringStream test_stream(m_testInput[m_thisIndex-1]);

        // ACT
        assertEqual(g_CmdParser->parse(test_stream), ParserState_Done);
        const Command* actual_result = g_CmdParser->command();

        // CHECK
        assertTrue(actual_result != NULL);
        assertEqual(actual_result->type, CommandType_SaveCfg);
        assertEqual(actual_result->argc, 1);
        assertTrue(actual_result->argv != NULL);
        const SaveCfgCommand* save_cfg_cmd =
            reinterpret_cast<const SaveCfgCommand*>(actual_result);
        assertEqual(save_cfg_cmd->params()->name, m_expectedParam[m_thisIndex-1].name);
        assertEqual(save_cfg_cmd->params()->effect, m_expectedParam[m_thisIndex-1].effect);
        assertEqual(save_cfg_cmd->params()->ticks, m_expectedParam[m_thisIndex-1].ticks);
        assertEqual(save_cfg_cmd->params()->extra[0], m_expectedParam[m_thisIndex-1].extra[0]);
        assertEqual(save_cfg_cmd->params()->extra[1], m_expectedParam[m_thisIndex-1].extra[1]);

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
            m_expectedParam[i].name = LedName_Green;
            switch (i)
            {
            case 0:
                m_testInput[i] = String(F("$save-cfg,1,g:x:11#"));
                break;
            case 1:
                m_testInput[i] = String(F("$save-cfg,1,g:o:22#"));
                break;
            case 2:
                m_testInput[i] = String(F("$save-cfg,1,g:/:33:-#"));
                break;
            case 3:
                m_testInput[i] = String(F("$save-cfg,1,g:/:44:e#"));
                break;
            case 4:
                m_testInput[i] = String(F("$save-cfg,1,g:\\:55:-#"));
                break;
            case 5:
                m_testInput[i] = String(F("$save-cfg,1,g:\\:66:e#"));
                break;
            }
        }
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
            m_expectedParam[i].name = LedName_Red;
            switch (i)
            {
            case 0:
                m_testInput[i] = String(F("$save-cfg,1,r:x:11#"));
                break;
            case 1:
                m_testInput[i] = String(F("$save-cfg,1,r:o:22#"));
                break;
            case 2:
                m_testInput[i] = String(F("$save-cfg,1,r:/:33:-#"));
                break;
            case 3:
                m_testInput[i] = String(F("$save-cfg,1,r:/:44:e#"));
                break;
            case 4:
                m_testInput[i] = String(F("$save-cfg,1,r:\\:55:-#"));
                break;
            case 5:
                m_testInput[i] = String(F("$save-cfg,1,r:\\:66:e#"));
                break;
            }
        }
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
            m_expectedParam[i].name = LedName_Blue;
            switch (i)
            {
            case 0:
                m_testInput[i] = String(F("$save-cfg,1,b:x:11#"));
                break;
            case 1:
                m_testInput[i] = String(F("$save-cfg,1,b:o:22#"));
                break;
            case 2:
                m_testInput[i] = String(F("$save-cfg,1,b:/:33:-#"));
                break;
            case 3:
                m_testInput[i] = String(F("$save-cfg,1,b:/:44:e#"));
                break;
            case 4:
                m_testInput[i] = String(F("$save-cfg,1,b:\\:55:-#"));
                break;
            case 5:
                m_testInput[i] = String(F("$save-cfg,1,b:\\:66:e#"));
                break;
            }
        }
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
            m_expectedParam[i].name = LedName_Yellow;
            switch (i)
            {
            case 0:
                m_testInput[i] = String(F("$save-cfg,1,y:x:11#"));
                break;
            case 1:
                m_testInput[i] = String(F("$save-cfg,1,y:o:22#"));
                break;
            case 2:
                m_testInput[i] = String(F("$save-cfg,1,y:/:33:-#"));
                break;
            case 3:
                m_testInput[i] = String(F("$save-cfg,1,y:/:44:e#"));
                break;
            case 4:
                m_testInput[i] = String(F("$save-cfg,1,y:\\:55:-#"));
                break;
            case 5:
                m_testInput[i] = String(F("$save-cfg,1,y:\\:66:e#"));
                break;
            }
        }
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
            m_expectedParam[i].name = LedName_White;
            switch (i)
            {
            case 0:
                m_testInput[i] = String(F("$save-cfg,1,w:x:11#"));
                break;
            case 1:
                m_testInput[i] = String(F("$save-cfg,1,w:o:22#"));
                break;
            case 2:
                m_testInput[i] = String(F("$save-cfg,1,w:/:33:-#"));
                break;
            case 3:
                m_testInput[i] = String(F("$save-cfg,1,w:/:44:e#"));
                break;
            case 4:
                m_testInput[i] = String(F("$save-cfg,1,w:\\:55:-#"));
                break;
            case 5:
                m_testInput[i] = String(F("$save-cfg,1,w:\\:66:e#"));
                break;
            }
        }
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
