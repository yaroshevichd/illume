#include <ArduinoUnit.h>
#include <IllumeUtils.h>

//struct SaveCfgTestPositiveTestOnce : TestOnce
//{
//    Command* m_actualResult;
//    SaveCfgCommand::SaveCfgParam m_expectedParam;
//
//    SaveCfgTestPositiveTestOnce(const char * test_name)
//        : TestOnce(test_name)
//        , m_actualResult(NULL)
//    {}
//
//    void once()
//    {
//        assertTrue(m_actualResult != NULL);
//        assertEqual(m_actualResult->type, CommandType_SaveCfg);
//        assertEqual(m_actualResult->argc, 1);
//        assertTrue(m_actualResult->argv != NULL);
//        SaveCfgCommand* save_cfg_cmd =
//            reinterpret_cast<SaveCfgCommand*>(m_actualResult);
//        assertEqual(save_cfg_cmd->params()->name, m_expectedParam.name);
//        assertEqual(save_cfg_cmd->params()->effect, m_expectedParam.effect);
//        assertEqual(save_cfg_cmd->params()->ticks, m_expectedParam.ticks);
//        assertEqual(save_cfg_cmd->params()->extra[0], m_expectedParam.extra[0]);
//        assertEqual(save_cfg_cmd->params()->extra[1], m_expectedParam.extra[1]);
//        Serial.print("Free: ");
//        Serial.println(freeMemory());
//    }
//};
//
//
//#pragma region GreenLedPositiveTests
//
//struct SaveCfgTestGreenPositiveTestOnce : SaveCfgTestPositiveTestOnce
//{
//    SaveCfgTestGreenPositiveTestOnce(const char * test_name)
//        : SaveCfgTestPositiveTestOnce(test_name)
//    {}
//
//    void once()
//    {
//        m_expectedParam.name = LedName_Green;
//        SaveCfgTestPositiveTestOnce::once();
//    }
//};
//
//struct save_cfg_parsed_if_green_led_on : SaveCfgTestGreenPositiveTestOnce
//{
//    save_cfg_parsed_if_green_led_on()
//        : SaveCfgTestGreenPositiveTestOnce("save_cfg_parsed_if_green_led_on")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 10;
//        m_expectedParam.effect = LedEffect_On;
//
//        String test_data("$save-cfg,1,g:x:10");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestGreenPositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_green_led_on_instance;
//
//struct save_cfg_parsed_if_green_led_off : SaveCfgTestGreenPositiveTestOnce
//{
//    save_cfg_parsed_if_green_led_off()
//        : SaveCfgTestGreenPositiveTestOnce("save_cfg_parsed_if_green_led_off")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 11;
//        m_expectedParam.effect = LedEffect_Off;
//
//        String test_data("$save-cfg,1,g:o:11");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestGreenPositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_green_led_off_instance;
//
//struct save_cfg_parsed_if_green_led_fade_in_linear : SaveCfgTestGreenPositiveTestOnce
//{
//    save_cfg_parsed_if_green_led_fade_in_linear()
//        : SaveCfgTestGreenPositiveTestOnce("save_cfg_parsed_if_green_led_fade_in_linear")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 12;
//        m_expectedParam.effect = LedEffect_FadeIn;
//        m_expectedParam.extra[0] = LedFadeType_Linear;
//
//        String test_data("$save-cfg,1,g:/:12:-");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestGreenPositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_green_led_fade_in_linear_instance;
//
//struct save_cfg_parsed_if_green_led_fade_in_exponential : SaveCfgTestGreenPositiveTestOnce
//{
//    save_cfg_parsed_if_green_led_fade_in_exponential()
//        : SaveCfgTestGreenPositiveTestOnce("save_cfg_parsed_if_green_led_fade_in_exponential")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 13;
//        m_expectedParam.effect = LedEffect_FadeIn;
//        m_expectedParam.extra[0] = LedFadeType_Exponential;
//
//        String test_data("$save-cfg,1,g:/:13:e");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestGreenPositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_green_led_fade_in_exponential_instance;
//
//struct save_cfg_parsed_if_green_led_fade_out_linear : SaveCfgTestGreenPositiveTestOnce
//{
//    save_cfg_parsed_if_green_led_fade_out_linear()
//        : SaveCfgTestGreenPositiveTestOnce("save_cfg_parsed_if_green_led_fade_out_linear")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 12;
//        m_expectedParam.effect = LedEffect_FadeOut;
//        m_expectedParam.extra[0] = LedFadeType_Linear;
//
//        String test_data("$save-cfg,1,g:\\:12:-");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestGreenPositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_green_led_fade_out_linear_instance;
//
//struct save_cfg_parsed_if_green_led_fade_out_exponential : SaveCfgTestGreenPositiveTestOnce
//{
//    save_cfg_parsed_if_green_led_fade_out_exponential()
//        : SaveCfgTestGreenPositiveTestOnce("save_cfg_parsed_if_green_led_fade_out_exponential")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 13;
//        m_expectedParam.effect = LedEffect_FadeOut;
//        m_expectedParam.extra[0] = LedFadeType_Exponential;
//
//        String test_data("$save-cfg,1,g:\\:13:e");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestGreenPositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_green_led_fade_out_exponential_instance;
//
//#pragma endregion
//
//
//#pragma region RedLedPositiveTests
//
//struct SaveCfgTestRedPositiveTestOnce : SaveCfgTestPositiveTestOnce
//{
//    SaveCfgTestRedPositiveTestOnce(const char * test_name)
//        : SaveCfgTestPositiveTestOnce(test_name)
//    {}
//
//    void once()
//    {
//        m_expectedParam.name = LedName_Red;
//        SaveCfgTestPositiveTestOnce::once();
//    }
//};
//
//struct save_cfg_parsed_if_red_led_on : SaveCfgTestRedPositiveTestOnce
//{
//    save_cfg_parsed_if_red_led_on()
//        : SaveCfgTestRedPositiveTestOnce("save_cfg_parsed_if_red_led_on")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 10;
//        m_expectedParam.effect = LedEffect_On;
//
//        String test_data("$save-cfg,1,r:x:10");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestRedPositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_red_led_on_instance;
//
//struct save_cfg_parsed_if_red_led_off : SaveCfgTestRedPositiveTestOnce
//{
//    save_cfg_parsed_if_red_led_off()
//        : SaveCfgTestRedPositiveTestOnce("save_cfg_parsed_if_red_led_off")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 11;
//        m_expectedParam.effect = LedEffect_Off;
//
//        String test_data("$save-cfg,1,r:o:11");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestRedPositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_red_led_off_instance;
//
//struct save_cfg_parsed_if_red_led_fade_in_linear : SaveCfgTestRedPositiveTestOnce
//{
//    save_cfg_parsed_if_red_led_fade_in_linear()
//        : SaveCfgTestRedPositiveTestOnce("save_cfg_parsed_if_red_led_fade_in_linear")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 12;
//        m_expectedParam.effect = LedEffect_FadeIn;
//        m_expectedParam.extra[0] = LedFadeType_Linear;
//
//        String test_data("$save-cfg,1,r:/:12:-");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestRedPositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_red_led_fade_in_linear_instance;
//
//struct save_cfg_parsed_if_red_led_fade_in_exponential : SaveCfgTestRedPositiveTestOnce
//{
//    save_cfg_parsed_if_red_led_fade_in_exponential()
//        : SaveCfgTestRedPositiveTestOnce("save_cfg_parsed_if_red_led_fade_in_exponential")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 13;
//        m_expectedParam.effect = LedEffect_FadeIn;
//        m_expectedParam.extra[0] = LedFadeType_Exponential;
//
//        String test_data("$save-cfg,1,r:/:13:e");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestRedPositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_red_led_fade_in_exponential_instance;
//
//struct save_cfg_parsed_if_red_led_fade_out_linear : SaveCfgTestRedPositiveTestOnce
//{
//    save_cfg_parsed_if_red_led_fade_out_linear()
//        : SaveCfgTestRedPositiveTestOnce("save_cfg_parsed_if_red_led_fade_out_linear")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 12;
//        m_expectedParam.effect = LedEffect_FadeOut;
//        m_expectedParam.extra[0] = LedFadeType_Linear;
//
//        String test_data("$save-cfg,1,r:\\:12:-");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestRedPositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_red_led_fade_out_linear_instance;
//
//struct save_cfg_parsed_if_red_led_fade_out_exponential : SaveCfgTestRedPositiveTestOnce
//{
//    save_cfg_parsed_if_red_led_fade_out_exponential()
//        : SaveCfgTestRedPositiveTestOnce("save_cfg_parsed_if_red_led_fade_out_exponential")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 13;
//        m_expectedParam.effect = LedEffect_FadeOut;
//        m_expectedParam.extra[0] = LedFadeType_Exponential;
//
//        String test_data("$save-cfg,1,r:\\:13:e");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestRedPositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_red_led_fade_out_exponential_instance;
//
//#pragma endregion


//#pragma region BlueLedPositiveTests
//
//struct SaveCfgTestBluePositiveTestOnce : SaveCfgTestPositiveTestOnce
//{
//    SaveCfgTestBluePositiveTestOnce(const char * test_name)
//        : SaveCfgTestPositiveTestOnce(test_name)
//    {}
//
//    void once()
//    {
//        m_expectedParam.name = LedName_Blue;
//        SaveCfgTestPositiveTestOnce::once();
//    }
//};
//
//struct save_cfg_parsed_if_blue_led_on : SaveCfgTestBluePositiveTestOnce
//{
//    save_cfg_parsed_if_blue_led_on()
//        : SaveCfgTestBluePositiveTestOnce("save_cfg_parsed_if_blue_led_on")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 10;
//        m_expectedParam.effect = LedEffect_On;
//
//        String test_data("$save-cfg,1,b:x:10");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestBluePositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_blue_led_on_instance;
//
//struct save_cfg_parsed_if_blue_led_off : SaveCfgTestBluePositiveTestOnce
//{
//    save_cfg_parsed_if_blue_led_off()
//        : SaveCfgTestBluePositiveTestOnce("save_cfg_parsed_if_blue_led_off")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 11;
//        m_expectedParam.effect = LedEffect_Off;
//
//        String test_data("$save-cfg,1,b:o:11");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestBluePositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_blue_led_off_instance;
//
//struct save_cfg_parsed_if_blue_led_fade_in_linear : SaveCfgTestBluePositiveTestOnce
//{
//    save_cfg_parsed_if_blue_led_fade_in_linear()
//        : SaveCfgTestBluePositiveTestOnce("save_cfg_parsed_if_blue_led_fade_in_linear")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 12;
//        m_expectedParam.effect = LedEffect_FadeIn;
//        m_expectedParam.extra[0] = LedFadeType_Linear;
//
//        String test_data("$save-cfg,1,b:/:12:-");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestBluePositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_blue_led_fade_in_linear_instance;
//
//struct save_cfg_parsed_if_blue_led_fade_in_exponential : SaveCfgTestBluePositiveTestOnce
//{
//    save_cfg_parsed_if_blue_led_fade_in_exponential()
//        : SaveCfgTestBluePositiveTestOnce("save_cfg_parsed_if_blue_led_fade_in_exponential")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 13;
//        m_expectedParam.effect = LedEffect_FadeIn;
//        m_expectedParam.extra[0] = LedFadeType_Exponential;
//
//        String test_data("$save-cfg,1,b:/:13:e");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestBluePositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_blue_led_fade_in_exponential_instance;
//
//struct save_cfg_parsed_if_blue_led_fade_out_linear : SaveCfgTestBluePositiveTestOnce
//{
//    save_cfg_parsed_if_blue_led_fade_out_linear()
//        : SaveCfgTestBluePositiveTestOnce("save_cfg_parsed_if_blue_led_fade_out_linear")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 12;
//        m_expectedParam.effect = LedEffect_FadeOut;
//        m_expectedParam.extra[0] = LedFadeType_Linear;
//
//        String test_data("$save-cfg,1,b:\\:12:-");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestBluePositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_blue_led_fade_out_linear_instance;
//
//struct save_cfg_parsed_if_blue_led_fade_out_exponential : SaveCfgTestBluePositiveTestOnce
//{
//    save_cfg_parsed_if_blue_led_fade_out_exponential()
//        : SaveCfgTestBluePositiveTestOnce("save_cfg_parsed_if_blue_led_fade_out_exponential")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 13;
//        m_expectedParam.effect = LedEffect_FadeOut;
//        m_expectedParam.extra[0] = LedFadeType_Exponential;
//
//        String test_data("$save-cfg,1,b:\\:13:e");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestBluePositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_blue_led_fade_out_exponential_instance;
//
//#pragma endregion


//#pragma region YellowLedPositiveTests
//
//struct SaveCfgTestYellowPositiveTestOnce : SaveCfgTestPositiveTestOnce
//{
//    SaveCfgTestYellowPositiveTestOnce(const char * test_name)
//        : SaveCfgTestPositiveTestOnce(test_name)
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
//        : SaveCfgTestYellowPositiveTestOnce("save_cfg_parsed_if_yellow_led_on")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 10;
//        m_expectedParam.effect = LedEffect_On;
//
//        String test_data("$save-cfg,1,y:x:10");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestYellowPositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_yellow_led_on_instance;
//
//struct save_cfg_parsed_if_yellow_led_off : SaveCfgTestYellowPositiveTestOnce
//{
//    save_cfg_parsed_if_yellow_led_off()
//        : SaveCfgTestYellowPositiveTestOnce("save_cfg_parsed_if_yellow_led_off")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 11;
//        m_expectedParam.effect = LedEffect_Off;
//
//        String test_data("$save-cfg,1,y:o:11");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestYellowPositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_yellow_led_off_instance;
//
//struct save_cfg_parsed_if_yellow_led_fade_in_linear : SaveCfgTestYellowPositiveTestOnce
//{
//    save_cfg_parsed_if_yellow_led_fade_in_linear()
//        : SaveCfgTestYellowPositiveTestOnce("save_cfg_parsed_if_yellow_led_fade_in_linear")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 12;
//        m_expectedParam.effect = LedEffect_FadeIn;
//        m_expectedParam.extra[0] = LedFadeType_Linear;
//
//        String test_data("$save-cfg,1,y:/:12:-");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestYellowPositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_yellow_led_fade_in_linear_instance;
//
//struct save_cfg_parsed_if_yellow_led_fade_in_exponential : SaveCfgTestYellowPositiveTestOnce
//{
//    save_cfg_parsed_if_yellow_led_fade_in_exponential()
//        : SaveCfgTestYellowPositiveTestOnce("save_cfg_parsed_if_yellow_led_fade_in_exponential")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 13;
//        m_expectedParam.effect = LedEffect_FadeIn;
//        m_expectedParam.extra[0] = LedFadeType_Exponential;
//
//        String test_data("$save-cfg,1,y:/:13:e");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestYellowPositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_yellow_led_fade_in_exponential_instance;
//
//struct save_cfg_parsed_if_yellow_led_fade_out_linear : SaveCfgTestYellowPositiveTestOnce
//{
//    save_cfg_parsed_if_yellow_led_fade_out_linear()
//        : SaveCfgTestYellowPositiveTestOnce("save_cfg_parsed_if_yellow_led_fade_out_linear")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 12;
//        m_expectedParam.effect = LedEffect_FadeOut;
//        m_expectedParam.extra[0] = LedFadeType_Linear;
//
//        String test_data("$save-cfg,1,y:\\:12:-");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestYellowPositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_yellow_led_fade_out_linear_instance;
//
//struct save_cfg_parsed_if_yellow_led_fade_out_exponential : SaveCfgTestYellowPositiveTestOnce
//{
//    save_cfg_parsed_if_yellow_led_fade_out_exponential()
//        : SaveCfgTestYellowPositiveTestOnce("save_cfg_parsed_if_yellow_led_fade_out_exponential")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 13;
//        m_expectedParam.effect = LedEffect_FadeOut;
//        m_expectedParam.extra[0] = LedFadeType_Exponential;
//
//        String test_data("$save-cfg,1,y:\\:13:e");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestYellowPositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_yellow_led_fade_out_exponential_instance;
//
//#pragma endregion


//#pragma region WhiteLedPositiveTests
//
//struct SaveCfgTestWhitePositiveTestOnce : SaveCfgTestPositiveTestOnce
//{
//    SaveCfgTestWhitePositiveTestOnce(const char * test_name)
//        : SaveCfgTestPositiveTestOnce(test_name)
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
//        : SaveCfgTestWhitePositiveTestOnce("save_cfg_parsed_if_white_led_on")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 10;
//        m_expectedParam.effect = LedEffect_On;
//
//        String test_data("$save-cfg,1,w:x:10");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestWhitePositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_white_led_on_instance;
//
//struct save_cfg_parsed_if_white_led_off : SaveCfgTestWhitePositiveTestOnce
//{
//    save_cfg_parsed_if_white_led_off()
//        : SaveCfgTestWhitePositiveTestOnce("save_cfg_parsed_if_white_led_off")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 11;
//        m_expectedParam.effect = LedEffect_Off;
//
//        String test_data("$save-cfg,1,w:o:11");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestWhitePositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_white_led_off_instance;
//
//struct save_cfg_parsed_if_white_led_fade_in_linear : SaveCfgTestWhitePositiveTestOnce
//{
//    save_cfg_parsed_if_white_led_fade_in_linear()
//        : SaveCfgTestWhitePositiveTestOnce("save_cfg_parsed_if_white_led_fade_in_linear")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 12;
//        m_expectedParam.effect = LedEffect_FadeIn;
//        m_expectedParam.extra[0] = LedFadeType_Linear;
//
//        String test_data("$save-cfg,1,w:/:12:-");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestWhitePositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_white_led_fade_in_linear_instance;
//
//struct save_cfg_parsed_if_white_led_fade_in_exponential : SaveCfgTestWhitePositiveTestOnce
//{
//    save_cfg_parsed_if_white_led_fade_in_exponential()
//        : SaveCfgTestWhitePositiveTestOnce("save_cfg_parsed_if_white_led_fade_in_exponential")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 13;
//        m_expectedParam.effect = LedEffect_FadeIn;
//        m_expectedParam.extra[0] = LedFadeType_Exponential;
//
//        String test_data("$save-cfg,1,w:/:13:e");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestWhitePositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_white_led_fade_in_exponential_instance;
//
//struct save_cfg_parsed_if_white_led_fade_out_linear : SaveCfgTestWhitePositiveTestOnce
//{
//    save_cfg_parsed_if_white_led_fade_out_linear()
//        : SaveCfgTestWhitePositiveTestOnce("save_cfg_parsed_if_white_led_fade_out_linear")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 12;
//        m_expectedParam.effect = LedEffect_FadeOut;
//        m_expectedParam.extra[0] = LedFadeType_Linear;
//
//        String test_data("$save-cfg,1,w:\\:12:-");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestWhitePositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_white_led_fade_out_linear_instance;
//
//struct save_cfg_parsed_if_white_led_fade_out_exponential : SaveCfgTestWhitePositiveTestOnce
//{
//    save_cfg_parsed_if_white_led_fade_out_exponential()
//        : SaveCfgTestWhitePositiveTestOnce("save_cfg_parsed_if_white_led_fade_out_exponential")
//    {}
//
//    void once()
//    {
//        m_expectedParam.ticks = 13;
//        m_expectedParam.effect = LedEffect_FadeOut;
//        m_expectedParam.extra[0] = LedFadeType_Exponential;
//
//        String test_data("$save-cfg,1,w:\\:13:e");
//        StringStream test_stream(test_data);
//        m_actualResult = readCommand(test_stream);
//
//        SaveCfgTestWhitePositiveTestOnce::once();
//    }
//} save_cfg_parsed_if_white_led_fade_out_exponential_instance;
//
//#pragma endregion


//#pragma region NegativeTests
//
//struct CmdTestNegativeTestOnce : TestOnce
//{
//    struct Command* m_actualResult;
//
//    CmdTestNegativeTestOnce(const char * test_name)
//        : TestOnce(test_name)
//        , m_actualResult(NULL)
//    {}
//
//    void once()
//    {
//        assertTrue(m_actualResult == NULL);
//    }
//};
//
//struct save_cfg_is_parsed_as_none_if_args_more_than_count : CmdTestNegativeTestOnce
//{
//    save_cfg_is_parsed_as_none_if_args_more_than_count()
//        : CmdTestNegativeTestOnce("save_cfg_is_parsed_as_none_if_args_more_than_count")
//    {}
//
//    void once()
//    {
//        String test_string("$save-cfg,0,bca");
//        StringStream test_stream(test_string);
//        m_actualResult = readCommand(test_stream);
//        CmdTestNegativeTestOnce::once();
//    }
//} save_cfg_is_parsed_as_none_if_args_more_than_count_instance;
//
//struct save_cfg_is_parsed_as_none_if_args_less_than_count : CmdTestNegativeTestOnce
//{
//    save_cfg_is_parsed_as_none_if_args_less_than_count()
//        : CmdTestNegativeTestOnce("save_cfg_is_parsed_as_none_if_args_less_than_count")
//    {}
//
//    void once()
//    {
//        String test_string("$save-cfg,2,bca");
//        StringStream test_stream(test_string);
//        m_actualResult = readCommand(test_stream);
//        CmdTestNegativeTestOnce::once();
//    }
//} save_cfg_is_parsed_as_none_if_args_less_than_count_instance;
//
//#pragma endregion


void setup()
{
    Serial.begin(9600);
    while(!Serial);
    Serial.print("Initial free: ");
    Serial.println(freeMemory());
}

void loop()
{
    Test::run();
}
