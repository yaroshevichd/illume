#line 2 "IllumeUtils_example.ino"
#include <ArduinoUnit.h>
#include <IllumeUtils.h>


#if (1) // region LoadCfgTests

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

#endif // endregion LoadCfgTests


struct SaveCfgTestPositiveTestOnce : TestOnce
{
    String m_testInput;
    SaveCfgCommand::SaveCfgParam m_expectedParam;

    SaveCfgTestPositiveTestOnce(
            const __FlashStringHelper * test_name
            , const __FlashStringHelper * test_data)
        : TestOnce(test_name)
        , m_testInput(test_data)
    {}

    void once()
    {
        // INIT
        StringStream test_stream(m_testInput);

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
        assertEqual(save_cfg_cmd->params()->name, m_expectedParam.name);
        assertEqual(save_cfg_cmd->params()->effect, m_expectedParam.effect);
        assertEqual(save_cfg_cmd->params()->ticks, m_expectedParam.ticks);
        assertEqual(save_cfg_cmd->params()->extra[0], m_expectedParam.extra[0]);
        assertEqual(save_cfg_cmd->params()->extra[1], m_expectedParam.extra[1]);
    }
};

#if (1) // begin GreenLedPositiveTests

struct SaveCfgTestGreenPositiveTestOnce : SaveCfgTestPositiveTestOnce
{
    SaveCfgTestGreenPositiveTestOnce(
            const __FlashStringHelper * test_name
            , const __FlashStringHelper * test_data)
        : SaveCfgTestPositiveTestOnce(test_name, test_data)
    {}

    void once()
    {
        m_expectedParam.name = LedName_Green;
        SaveCfgTestPositiveTestOnce::once();
    }
};

struct save_cfg_parsed_if_green_led_on : SaveCfgTestGreenPositiveTestOnce
{
    save_cfg_parsed_if_green_led_on()
        : SaveCfgTestGreenPositiveTestOnce(F("save_cfg_parsed_if_green_led_on"), F("$save-cfg,1,g:x:10#"))
    {}

    void once()
    {
        m_expectedParam.ticks = 10;
        m_expectedParam.effect = LedEffect_On;
        SaveCfgTestGreenPositiveTestOnce::once();
    }
} save_cfg_parsed_if_green_led_on_instance;

struct save_cfg_parsed_if_green_led_off : SaveCfgTestGreenPositiveTestOnce
{
    save_cfg_parsed_if_green_led_off()
        : SaveCfgTestGreenPositiveTestOnce(F("save_cfg_parsed_if_green_led_off"), F("$save-cfg,1,g:o:11#"))
    {}

    void once()
    {
        m_expectedParam.ticks = 11;
        m_expectedParam.effect = LedEffect_Off;
        SaveCfgTestGreenPositiveTestOnce::once();
    }
} save_cfg_parsed_if_green_led_off_instance;

struct save_cfg_parsed_if_green_led_fade_in_linear : SaveCfgTestGreenPositiveTestOnce
{
    save_cfg_parsed_if_green_led_fade_in_linear()
        : SaveCfgTestGreenPositiveTestOnce(F("save_cfg_parsed_if_green_led_fade_in_linear"), F("$save-cfg,1,g:/:12:-#"))
    {}

    void once()
    {
        m_expectedParam.ticks = 12;
        m_expectedParam.effect = LedEffect_FadeIn;
        m_expectedParam.extra[0] = LedFadeType_Linear;
        SaveCfgTestGreenPositiveTestOnce::once();
    }
} save_cfg_parsed_if_green_led_fade_in_linear_instance;

struct save_cfg_parsed_if_green_led_fade_in_exponential : SaveCfgTestGreenPositiveTestOnce
{
    save_cfg_parsed_if_green_led_fade_in_exponential()
        : SaveCfgTestGreenPositiveTestOnce(F("save_cfg_parsed_if_green_led_fade_in_exponential"), F("$save-cfg,1,g:/:13:e#"))
    {}

    void once()
    {
        m_expectedParam.ticks = 13;
        m_expectedParam.effect = LedEffect_FadeIn;
        m_expectedParam.extra[0] = LedFadeType_Exponential;
        SaveCfgTestGreenPositiveTestOnce::once();
    }
} save_cfg_parsed_if_green_led_fade_in_exponential_instance;

struct save_cfg_parsed_if_green_led_fade_out_linear : SaveCfgTestGreenPositiveTestOnce
{
    save_cfg_parsed_if_green_led_fade_out_linear()
        : SaveCfgTestGreenPositiveTestOnce(F("save_cfg_parsed_if_green_led_fade_out_linear"), F("$save-cfg,1,g:\\:12:-#"))
    {}

    void once()
    {
        m_expectedParam.ticks = 12;
        m_expectedParam.effect = LedEffect_FadeOut;
        m_expectedParam.extra[0] = LedFadeType_Linear;
        SaveCfgTestGreenPositiveTestOnce::once();
    }
} save_cfg_parsed_if_green_led_fade_out_linear_instance;

struct save_cfg_parsed_if_green_led_fade_out_exponential : SaveCfgTestGreenPositiveTestOnce
{
    save_cfg_parsed_if_green_led_fade_out_exponential()
        : SaveCfgTestGreenPositiveTestOnce(F("save_cfg_parsed_if_green_led_fade_out_exponential"), F("$save-cfg,1,g:\\:13:e#"))
    {}

    void once()
    {
        m_expectedParam.ticks = 13;
        m_expectedParam.effect = LedEffect_FadeOut;
        m_expectedParam.extra[0] = LedFadeType_Exponential;
        SaveCfgTestGreenPositiveTestOnce::once();
    }
} save_cfg_parsed_if_green_led_fade_out_exponential_instance;

#endif // end GreenLedPositiveTests


#if (1) // begin RedLedPositiveTests

struct SaveCfgTestRedPositiveTestOnce : SaveCfgTestPositiveTestOnce
{
    SaveCfgTestRedPositiveTestOnce(
            const __FlashStringHelper * test_name,
            const __FlashStringHelper * test_data)
        : SaveCfgTestPositiveTestOnce(test_name, test_data)
    {}

    void once()
    {
        m_expectedParam.name = LedName_Red;
        SaveCfgTestPositiveTestOnce::once();
    }
};

struct save_cfg_parsed_if_red_led_on : SaveCfgTestRedPositiveTestOnce
{
    save_cfg_parsed_if_red_led_on()
        : SaveCfgTestRedPositiveTestOnce(F("save_cfg_parsed_if_red_led_on"), F("$save-cfg,1,r:x:10#"))
    {}

    void once()
    {
        m_expectedParam.ticks = 10;
        m_expectedParam.effect = LedEffect_On;
        SaveCfgTestRedPositiveTestOnce::once();
    }
} save_cfg_parsed_if_red_led_on_instance;

struct save_cfg_parsed_if_red_led_off : SaveCfgTestRedPositiveTestOnce
{
    save_cfg_parsed_if_red_led_off()
        : SaveCfgTestRedPositiveTestOnce(F("save_cfg_parsed_if_red_led_off"), F("$save-cfg,1,r:o:11#"))
    {}

    void once()
    {
        m_expectedParam.ticks = 11;
        m_expectedParam.effect = LedEffect_Off;
        SaveCfgTestRedPositiveTestOnce::once();
    }
} save_cfg_parsed_if_red_led_off_instance;

struct save_cfg_parsed_if_red_led_fade_in_linear : SaveCfgTestRedPositiveTestOnce
{
    save_cfg_parsed_if_red_led_fade_in_linear()
        : SaveCfgTestRedPositiveTestOnce(F("save_cfg_parsed_if_red_led_fade_in_linear"), F("$save-cfg,1,r:/:12:-#"))
    {}

    void once()
    {
        m_expectedParam.ticks = 12;
        m_expectedParam.effect = LedEffect_FadeIn;
        m_expectedParam.extra[0] = LedFadeType_Linear;
        SaveCfgTestRedPositiveTestOnce::once();
    }
} save_cfg_parsed_if_red_led_fade_in_linear_instance;

struct save_cfg_parsed_if_red_led_fade_in_exponential : SaveCfgTestRedPositiveTestOnce
{
    save_cfg_parsed_if_red_led_fade_in_exponential()
        : SaveCfgTestRedPositiveTestOnce(F("save_cfg_parsed_if_red_led_fade_in_exponential"), F("$save-cfg,1,r:/:13:e#"))
    {}

    void once()
    {
        m_expectedParam.ticks = 13;
        m_expectedParam.effect = LedEffect_FadeIn;
        m_expectedParam.extra[0] = LedFadeType_Exponential;
        SaveCfgTestRedPositiveTestOnce::once();
    }
} save_cfg_parsed_if_red_led_fade_in_exponential_instance;

struct save_cfg_parsed_if_red_led_fade_out_linear : SaveCfgTestRedPositiveTestOnce
{
    save_cfg_parsed_if_red_led_fade_out_linear()
        : SaveCfgTestRedPositiveTestOnce(F("save_cfg_parsed_if_red_led_fade_out_linear"), F("$save-cfg,1,r:\\:12:-#"))
    {}

    void once()
    {
        m_expectedParam.ticks = 12;
        m_expectedParam.effect = LedEffect_FadeOut;
        m_expectedParam.extra[0] = LedFadeType_Linear;
        SaveCfgTestRedPositiveTestOnce::once();
    }
} save_cfg_parsed_if_red_led_fade_out_linear_instance;

struct save_cfg_parsed_if_red_led_fade_out_exponential : SaveCfgTestRedPositiveTestOnce
{
    save_cfg_parsed_if_red_led_fade_out_exponential()
        : SaveCfgTestRedPositiveTestOnce(F("save_cfg_parsed_if_red_led_fade_out_exponential"), F("$save-cfg,1,r:\\:13:e#"))
    {}

    void once()
    {
        m_expectedParam.ticks = 13;
        m_expectedParam.effect = LedEffect_FadeOut;
        m_expectedParam.extra[0] = LedFadeType_Exponential;
        SaveCfgTestRedPositiveTestOnce::once();
    }
} save_cfg_parsed_if_red_led_fade_out_exponential_instance;

#endif // end RedLedPositiveTests


#if (1) // begin BlueLedPositiveTests

struct SaveCfgTestBluePositiveTestOnce : SaveCfgTestPositiveTestOnce
{
    SaveCfgTestBluePositiveTestOnce(
            const __FlashStringHelper * test_name,
            const __FlashStringHelper * test_data)
        : SaveCfgTestPositiveTestOnce(test_name, test_data)
    {}

    void once()
    {
        m_expectedParam.name = LedName_Blue;
        SaveCfgTestPositiveTestOnce::once();
    }
};

struct save_cfg_parsed_if_blue_led_on : SaveCfgTestBluePositiveTestOnce
{
    save_cfg_parsed_if_blue_led_on()
        : SaveCfgTestBluePositiveTestOnce(F("save_cfg_parsed_if_blue_led_on"), F("$save-cfg,1,b:x:10#"))
    {}

    void once()
    {
        m_expectedParam.ticks = 10;
        m_expectedParam.effect = LedEffect_On;
        SaveCfgTestBluePositiveTestOnce::once();
    }
} save_cfg_parsed_if_blue_led_on_instance;

struct save_cfg_parsed_if_blue_led_off : SaveCfgTestBluePositiveTestOnce
{
    save_cfg_parsed_if_blue_led_off()
        : SaveCfgTestBluePositiveTestOnce(F("save_cfg_parsed_if_blue_led_off"), F("$save-cfg,1,b:o:11#"))
    {}

    void once()
    {
        m_expectedParam.ticks = 11;
        m_expectedParam.effect = LedEffect_Off;
        SaveCfgTestBluePositiveTestOnce::once();
    }
} save_cfg_parsed_if_blue_led_off_instance;

struct save_cfg_parsed_if_blue_led_fade_in_linear : SaveCfgTestBluePositiveTestOnce
{
    save_cfg_parsed_if_blue_led_fade_in_linear()
        : SaveCfgTestBluePositiveTestOnce(F("save_cfg_parsed_if_blue_led_fade_in_linear"), F("$save-cfg,1,b:/:12:-#"))
    {}

    void once()
    {
        m_expectedParam.ticks = 12;
        m_expectedParam.effect = LedEffect_FadeIn;
        m_expectedParam.extra[0] = LedFadeType_Linear;
        SaveCfgTestBluePositiveTestOnce::once();
    }
} save_cfg_parsed_if_blue_led_fade_in_linear_instance;

struct save_cfg_parsed_if_blue_led_fade_in_exponential : SaveCfgTestBluePositiveTestOnce
{
    save_cfg_parsed_if_blue_led_fade_in_exponential()
        : SaveCfgTestBluePositiveTestOnce(F("save_cfg_parsed_if_blue_led_fade_in_exponential"), F("$save-cfg,1,b:/:13:e#"))
    {}

    void once()
    {
        m_expectedParam.ticks = 13;
        m_expectedParam.effect = LedEffect_FadeIn;
        m_expectedParam.extra[0] = LedFadeType_Exponential;
        SaveCfgTestBluePositiveTestOnce::once();
    }
} save_cfg_parsed_if_blue_led_fade_in_exponential_instance;

struct save_cfg_parsed_if_blue_led_fade_out_linear : SaveCfgTestBluePositiveTestOnce
{
    save_cfg_parsed_if_blue_led_fade_out_linear()
        : SaveCfgTestBluePositiveTestOnce(F("save_cfg_parsed_if_blue_led_fade_out_linear"), F("$save-cfg,1,b:\\:12:-#"))
    {}

    void once()
    {
        m_expectedParam.ticks = 12;
        m_expectedParam.effect = LedEffect_FadeOut;
        m_expectedParam.extra[0] = LedFadeType_Linear;
        SaveCfgTestBluePositiveTestOnce::once();
    }
} save_cfg_parsed_if_blue_led_fade_out_linear_instance;

struct save_cfg_parsed_if_blue_led_fade_out_exponential : SaveCfgTestBluePositiveTestOnce
{
    save_cfg_parsed_if_blue_led_fade_out_exponential()
        : SaveCfgTestBluePositiveTestOnce(F("save_cfg_parsed_if_blue_led_fade_out_exponential"), F("$save-cfg,1,b:\\:13:e#"))
    {}

    void once()
    {
        m_expectedParam.ticks = 13;
        m_expectedParam.effect = LedEffect_FadeOut;
        m_expectedParam.extra[0] = LedFadeType_Exponential;
        SaveCfgTestBluePositiveTestOnce::once();
    }
} save_cfg_parsed_if_blue_led_fade_out_exponential_instance;

#endif // end BlueLedPositiveTests


//#if (1) // begin YellowLedPositiveTests
//
//struct SaveCfgTestYellowPositiveTestOnce : SaveCfgTestPositiveTestOnce
//{
//    SaveCfgTestYellowPositiveTestOnce(
//            const __FlashStringHelper * test_name,
//            const __FlashStringHelper * test_data)
//        : SaveCfgTestPositiveTestOnce(test_name, test_data)
//    {}
//
//    void once()
//    {
//        m_expectedParam.name = LedName_Yellow;
//        SaveCfgTestPositiveTestOnce::once();
//    }
//};
//
//struct save_cfg_parsed_if_yellow_led_on : SaveCfgTestYellowPositiveTestOnce
//{
//    save_cfg_parsed_if_yellow_led_on()
//        : SaveCfgTestYellowPositiveTestOnce(F("save_cfg_parsed_if_yellow_led_on"), F("$save-cfg,1,y:x:10#"))
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 10;
//        m_expectedParam.effect = LedEffect_On;
//        SaveCfgTestYellowPositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_yellow_led_on_instance;
//
//struct save_cfg_parsed_if_yellow_led_off : SaveCfgTestYellowPositiveTestOnce
//{
//    save_cfg_parsed_if_yellow_led_off()
//        : SaveCfgTestYellowPositiveTestOnce(F("save_cfg_parsed_if_yellow_led_off"), F("$save-cfg,1,y:o:11#"))
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 11;
//        m_expectedParam.effect = LedEffect_Off;
//        SaveCfgTestYellowPositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_yellow_led_off_instance;
//
//struct save_cfg_parsed_if_yellow_led_fade_in_linear : SaveCfgTestYellowPositiveTestOnce
//{
//    save_cfg_parsed_if_yellow_led_fade_in_linear()
//        : SaveCfgTestYellowPositiveTestOnce(F("save_cfg_parsed_if_yellow_led_fade_in_linear"), F("$save-cfg,1,y:/:12:-#"))
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 12;
//        m_expectedParam.effect = LedEffect_FadeIn;
//        m_expectedParam.extra[0] = LedFadeType_Linear;
//        SaveCfgTestYellowPositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_yellow_led_fade_in_linear_instance;
//
//struct save_cfg_parsed_if_yellow_led_fade_in_exponential : SaveCfgTestYellowPositiveTestOnce
//{
//    save_cfg_parsed_if_yellow_led_fade_in_exponential()
//        : SaveCfgTestYellowPositiveTestOnce(F("save_cfg_parsed_if_yellow_led_fade_in_exponential"), F("$save-cfg,1,y:/:13:e#"))
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 13;
//        m_expectedParam.effect = LedEffect_FadeIn;
//        m_expectedParam.extra[0] = LedFadeType_Exponential;
//        SaveCfgTestYellowPositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_yellow_led_fade_in_exponential_instance;
//
//struct save_cfg_parsed_if_yellow_led_fade_out_linear : SaveCfgTestYellowPositiveTestOnce
//{
//    save_cfg_parsed_if_yellow_led_fade_out_linear()
//        : SaveCfgTestYellowPositiveTestOnce(F("save_cfg_parsed_if_yellow_led_fade_out_linear"), F("$save-cfg,1,y:\\:12:-#"))
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 12;
//        m_expectedParam.effect = LedEffect_FadeOut;
//        m_expectedParam.extra[0] = LedFadeType_Linear;
//        SaveCfgTestYellowPositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_yellow_led_fade_out_linear_instance;
//
//struct save_cfg_parsed_if_yellow_led_fade_out_exponential : SaveCfgTestYellowPositiveTestOnce
//{
//    save_cfg_parsed_if_yellow_led_fade_out_exponential()
//        : SaveCfgTestYellowPositiveTestOnce(F("save_cfg_parsed_if_yellow_led_fade_out_exponential"), F("$save-cfg,1,y:\\:13:e#"))
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 13;
//        m_expectedParam.effect = LedEffect_FadeOut;
//        m_expectedParam.extra[0] = LedFadeType_Exponential;
//        SaveCfgTestYellowPositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_yellow_led_fade_out_exponential_instance;
//
//#endif // end YellowLedPositiveTests


//#if (1) // begin WhiteLedPositiveTests
//
//struct SaveCfgTestWhitePositiveTestOnce : SaveCfgTestPositiveTestOnce
//{
//    SaveCfgTestWhitePositiveTestOnce(
//            const __FlashStringHelper * test_name,
//            const __FlashStringHelper * test_data)
//        : SaveCfgTestPositiveTestOnce(test_name, test_data)
//    {}
//
//    void once()
//    {
//        m_expectedParam.name = LedName_White;
//        SaveCfgTestPositiveTestOnce::once();
//    }
//};
//
//struct save_cfg_parsed_if_white_led_on : SaveCfgTestWhitePositiveTestOnce
//{
//    save_cfg_parsed_if_white_led_on()
//        : SaveCfgTestWhitePositiveTestOnce(F("save_cfg_parsed_if_white_led_on"), F("$save-cfg,1,w:x:10#"))
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 10;
//        m_expectedParam.effect = LedEffect_On;
//        SaveCfgTestWhitePositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_white_led_on_instance;
//
//struct save_cfg_parsed_if_white_led_off : SaveCfgTestWhitePositiveTestOnce
//{
//    save_cfg_parsed_if_white_led_off()
//        : SaveCfgTestWhitePositiveTestOnce(F("save_cfg_parsed_if_white_led_off"), F("$save-cfg,1,w:o:11#"))
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 11;
//        m_expectedParam.effect = LedEffect_Off;
//        SaveCfgTestWhitePositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_white_led_off_instance;
//
//struct save_cfg_parsed_if_white_led_fade_in_linear : SaveCfgTestWhitePositiveTestOnce
//{
//    save_cfg_parsed_if_white_led_fade_in_linear()
//        : SaveCfgTestWhitePositiveTestOnce(F("save_cfg_parsed_if_white_led_fade_in_linear"), F("$save-cfg,1,w:/:12:-#"))
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 12;
//        m_expectedParam.effect = LedEffect_FadeIn;
//        m_expectedParam.extra[0] = LedFadeType_Linear;
//        SaveCfgTestWhitePositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_white_led_fade_in_linear_instance;
//
//struct save_cfg_parsed_if_white_led_fade_in_exponential : SaveCfgTestWhitePositiveTestOnce
//{
//    save_cfg_parsed_if_white_led_fade_in_exponential()
//        : SaveCfgTestWhitePositiveTestOnce(F("save_cfg_parsed_if_white_led_fade_in_exponential"), F("$save-cfg,1,w:/:13:e#"))
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 13;
//        m_expectedParam.effect = LedEffect_FadeIn;
//        m_expectedParam.extra[0] = LedFadeType_Exponential;
//        SaveCfgTestWhitePositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_white_led_fade_in_exponential_instance;
//
//struct save_cfg_parsed_if_white_led_fade_out_linear : SaveCfgTestWhitePositiveTestOnce
//{
//    save_cfg_parsed_if_white_led_fade_out_linear()
//        : SaveCfgTestWhitePositiveTestOnce(F("save_cfg_parsed_if_white_led_fade_out_linear"), F("$save-cfg,1,w:\\:12:-#"))
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 12;
//        m_expectedParam.effect = LedEffect_FadeOut;
//        m_expectedParam.extra[0] = LedFadeType_Linear;
//        SaveCfgTestWhitePositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_white_led_fade_out_linear_instance;
//
//struct save_cfg_parsed_if_white_led_fade_out_exponential : SaveCfgTestWhitePositiveTestOnce
//{
//    save_cfg_parsed_if_white_led_fade_out_exponential()
//        : SaveCfgTestWhitePositiveTestOnce(F("save_cfg_parsed_if_white_led_fade_out_exponential"), F("$save-cfg,1,w:\\:13:e#"))
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 13;
//        m_expectedParam.effect = LedEffect_FadeOut;
//        m_expectedParam.extra[0] = LedFadeType_Exponential;
//        SaveCfgTestWhitePositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_white_led_fade_out_exponential_instance;
//
//#endif // end WhiteLedPositiveTests


void setup()
{
    Serial.begin(9600);
    while(!Serial);
}

void loop()
{
    static bool firstRun = true;

    if (firstRun)
    {
        Serial.print("--> ");
        Serial.println(freeMemory());
    }

    Test::run();

    if (firstRun)
    {
        Serial.print("<-- ");
        Serial.println(freeMemory());
    }

    firstRun = false;
}
