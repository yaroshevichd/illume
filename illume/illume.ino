#include <EEPROM.h>
#include <TaskScheduler.h>
#include <ArduinoUnit.h>
#include <Illume_CommandParsers.h>
#include <Illume_LedProcessor.h>

/**
 * Constants
 */

#define MAX_ENTRIES (256)
//#define MAX_ENTRIES (1)
#define LEDS_COUNT (5)
#define LED_PIN_GREEN (3)
#define LED_PIN_YELLOW (5)
#define LED_PIN_BLUE (9)
#define LED_PIN_WHITE (10)
#define LED_PIN_RED (11)

/**
 * Macros
 */

#define LEN(x) (sizeof(x)/sizeof(x[0]))
#define OFFSETOF(type, field) ((unsigned) &(((type *) 0)->field))

/**
 * Forward declarations
 */


/**
 * Globals
 */
Scheduler g_taskManager;
Task g_taskInit(0, 1, &initHandler, &g_taskManager, true);
Task g_taskProcessLeds(15, -1, &processLedsHandler, &g_taskManager, false);
Task g_taskProcessCommand(50, -1, &processCommandHandler, &g_taskManager, false);
LedProcessor * const g_LedsProcessors[] = {
    createLedProcessor(LED_PIN_RED, LedName_Red),
    createLedProcessor(LED_PIN_GREEN, LedName_Green),
    createLedProcessor(LED_PIN_BLUE, LedName_Blue),
    createLedProcessor(LED_PIN_YELLOW, LedName_Yellow),
    createLedProcessor(LED_PIN_WHITE, LedName_White)
};
struct Configuration
{
    struct
    {
        byte tick_ms:4;
        byte reading_ticks:4;
    } durations;
    byte entries;
    LedAnimationStep steps[MAX_ENTRIES];
} g_Config;

/*
 * Tasks handlers
 */
void processLedsHandler()
{
    for (byte i = 0; i < LEN(g_LedsProcessors); ++i)
    {
        g_LedsProcessors[i]->process();
    }
}

void resetConfig()
{
    memset(&g_Config, 0, sizeof(g_Config));
    g_Config.durations.tick_ms = 15;
    g_Config.durations.reading_ticks = 4;
    g_Config.entries = 0;
    EEPROM.put(0, g_Config);
}

void loadLedsConfig()
{
    Serial.println(F("Configuration:"));
    Serial.print(F("  Tick (ms): "));
    Serial.println(g_Config.durations.tick_ms);
    Serial.print(F("  Reading (ticks): "));
    Serial.println(g_Config.durations.reading_ticks);
    Serial.print(F("  Entries: "));
    Serial.println(g_Config.entries);

    for (byte i = 0; i < g_Config.entries; ++i)
    {
        Serial.print(F("    led="));
        Serial.print(LedNameToString(g_Config.steps[i].info.name));
        Serial.print(F("; state="));
        Serial.print(LedAnimationToString(g_Config.steps[i].info.animation));
        Serial.print(F("; factor="));
        Serial.print(LedAnimationFactorToString(g_Config.steps[i].info.factor));
        if (g_Config.steps[i].info.once)
        {
            Serial.println(F("; once"));
        }
        Serial.print(F("; duration="));
        Serial.println(g_Config.steps[i].duration);
    }

    g_taskProcessLeds.setInterval(g_Config.durations.tick_ms);
    g_taskProcessCommand.setInterval(
        g_Config.durations.tick_ms * g_Config.durations.reading_ticks
    );

    for (byte i = 0; i < LEN(g_LedsProcessors); ++i)
    {
        g_LedsProcessors[i]->init();
        g_LedsProcessors[i]->apply(g_Config.steps, g_Config.entries);
    }

    g_taskProcessLeds.enable();
    g_taskProcessCommand.enable();
}

void initHandler()
{
    EEPROM.get(0, g_Config);
    if (g_Config.entries > MAX_ENTRIES)
    {
        resetConfig();
    }
    loadLedsConfig();
}

void saveLedsConfig(const SaveCfgCommand::SaveCfgParam* steps, const byte stepsCount)
{
    g_Config.entries = stepsCount;
    for (int i = 0; i < stepsCount; ++i)
    {
        g_Config.steps[i] = steps[i];
    }
    EEPROM.put(0, g_Config);

    for (byte i = 0; i < LEN(g_LedsProcessors); ++i)
    {
        g_LedsProcessors[i]->apply(g_Config.steps, g_Config.entries);
    }
}

CommandParser * const parser = getTextualCommandParser();
void processCommandHandler()
{
    if (Serial.available() > 0)
    {
        if (parser->parse(Serial) == ParserState_Done)
        {
            const Command* cmd = parser->command();
            switch (cmd->type)
            {
            case CommandType_SaveCfg:
                {
                    SaveCfgCommand::SaveCfgParam* save_cfg_params =
                        reinterpret_cast<SaveCfgCommand::SaveCfgParam*>(cmd->argv);
                    saveLedsConfig(save_cfg_params, cmd->argc);
                }
                break;
            default:
                break;
            }
            parser->reset();
        }
    }
}


void setup()
{
    Serial.begin(9600);
}

void loop()
{
    g_taskManager.execute();
}
