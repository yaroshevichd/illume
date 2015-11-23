//#define _DEBUG
#include <TaskScheduler.h>
#include <IllumeUtils.h>
#include <ArduinoUnit.h>

#define LEN(x) (sizeof(x)/sizeof(x[0]))

struct LedPin
{
    typedef byte Type;

    static const Type Green = 3;
    static const Type Orange = 5;
    static const Type Blue = 9;
    static const Type White = 10;
    static const Type Red = 11;

#ifdef _DEBUG
    static const char * toString(const LedPin::Type value)
    {
        switch(value)
        {
            case LedPin::Green: return "green";
            case LedPin::Orange: return "orange";
            case LedPin::Blue: return "blue";
            case LedPin::White: return "white";
            case LedPin::Red: return "red";
        }
        return "undefined";
    }
#endif
};

struct LedState
{
    typedef byte Type;

    static const Type On = 0;
    static const Type Off = 1;
    static const Type FadeIn = 2;
    static const Type FadeOut = 3;

#ifdef _DEBUG
    static const char * toString(const LedState::Type value)
    {
        switch(value)
        {
            case LedState::On: return "on";
            case LedState::Off: return "off";
            case LedState::FadeIn: return "fade-in";
            case LedState::FadeOut: return "fade-out";
        }
        return "undefined";
    }
#endif
};


struct LedPwmFactors
{
    typedef byte Type;

    static const Type Quarter = 0;
    static const Type Half = 1;
    static const Type Once = 2;
    static const Type Twice = 3;

#ifdef _DEBUG
    static const char * toString(const LedPwmFactors::Type value)
    {
        switch(value)
        {
            case LedPwmFactors::Quarter: return "quarter";
            case LedPwmFactors::Half: return "half";
            case LedPwmFactors::Once: return "once";
            case LedPwmFactors::Twice: return "twice";
        }
        return "undefined";
    }
#endif
};


struct LedStep
{
    struct
    {
        byte led:3;
        byte state:2;
        byte factor:2;
        byte once:1;
    } info;
    int duration;
    
    LedStep()
      : duration(0)
    {
        info.state = LedState::Off;
    }
    
    LedStep(const LedState::Type state, const int ticks, const LedPwmFactors::Type factor = LedPwmFactors::Once, const bool once = false)
      : duration(ticks)
    {
        info.led = 1;
        info.state = state;
        info.factor = factor;
        info.once = once;
    }
};

struct LedRuntimeInfo
{
    float pwm;
    LedState::Type state;
    LedState::Type lastState;
    int ticks;
    int step;

    LedRuntimeInfo()
        : pwm(0)
        , state(LedState::On)
        , lastState(LedState::On)
        , ticks(0)
        , step(0)
    {}
};

struct LedInfo
{
    LedPin::Type pin;
    const LedStep* steps;
    int stepsCount;
    LedRuntimeInfo runtime;

    LedInfo(const int pin, const LedStep* steps, const int steps_count)
        : pin(pin)
        , steps(steps)
        , stepsCount(steps_count)
    {
        runtime.state = steps[0].info.state;
        runtime.ticks = steps[0].duration;
    }

    void apply(const LedStep* steps, const int steps_count)
    {
        steps = steps;
        stepsCount = steps_count;
        runtime = LedRuntimeInfo();
        runtime.state = steps[0].info.state;
        runtime.ticks = steps[0].duration;
    }

    inline int currentStep() const
    {
        return runtime.step;
    }

    void nextStep()
    {
        do
        {
            runtime.step = (currentStep() + 1) % stepsCount;
        }
        while (steps[currentStep()].info.once);
        runtime.lastState = runtime.state;
        runtime.state = steps[currentStep()].info.state;
        runtime.ticks = steps[currentStep()].duration;
    }

    bool isNextStep()
    {
        return --runtime.ticks < 0;
    }

    void process()
    {
        switch (runtime.state)
        {
        case LedState::On:
            if (runtime.lastState != LedState::On)
            {
                analogWrite(pin, runtime.pwm);
                runtime.lastState = LedState::On;
            }
            break;
        case LedState::Off:
            if (runtime.lastState != LedState::Off)
            {
                analogWrite(pin, 0);
                runtime.lastState = LedState::Off;
            }
            break;
        case LedState::FadeIn:
            if (runtime.lastState != LedState::FadeIn)
            {
                runtime.lastState = LedState::FadeIn;
            }
            analogWrite(pin, (int)runtime.pwm);
            switch (steps[runtime.step].info.factor)
            {
            case LedPwmFactors::Quarter: runtime.pwm += 0.25; break;
            case LedPwmFactors::Half: runtime.pwm += 0.5; break;
            case LedPwmFactors::Once: runtime.pwm += 1.0; break;
            case LedPwmFactors::Twice: runtime.pwm += 2.0; break;
            }
            break;
        case LedState::FadeOut:
            if (runtime.lastState != LedState::FadeOut)
            {
                runtime.lastState = LedState::FadeOut;
            }
            analogWrite(pin, (int)runtime.pwm);
            switch (steps[runtime.step].info.factor)
            {
            case LedPwmFactors::Quarter: runtime.pwm -= 0.25; break;
            case LedPwmFactors::Half: runtime.pwm -= 0.5; break;
            case LedPwmFactors::Once: runtime.pwm -= 1.0; break;
            case LedPwmFactors::Twice: runtime.pwm -= 2.0; break;
            }
            break;
        }
        if (isNextStep())
        {
            nextStep();
#ifdef _DEBUG
            debug();
#endif
        }
    }

#ifdef _DEBUG
    void debug() const
    {
        Serial.print(F("Led '"));
        Serial.print(LedPin::toString(this->pin));
        Serial.print(F("' transit from '"));
        Serial.print(LedState::toString(this->runtime.lastState));
        Serial.print(F("' to '"));
        Serial.print(LedState::toString(this->runtime.state));
        Serial.print(F("': ticks="));
        Serial.print(this->runtime.ticks);
        Serial.print(F(", pwm="));
        Serial.println(this->runtime.pwm);
    }
#endif
};

LedStep g_greenSteps[] = {
    LedStep(LedState::FadeIn, 128),
    LedStep(LedState::FadeOut, 128),
    LedStep(LedState::Off, 128),
    LedStep(LedState::FadeIn, 128),
    LedStep(LedState::On, 640),
    LedStep(LedState::FadeOut, 128),
};

LedStep g_redSteps[] = {
    LedStep(LedState::Off, 128),
    LedStep(LedState::FadeIn, 128),
    LedStep(LedState::FadeOut, 128),
    LedStep(LedState::Off, 128),
    LedStep(LedState::FadeIn, 128),
    LedStep(LedState::On, 384),
    LedStep(LedState::FadeOut, 128),
    LedStep(LedState::Off, 128),
};

LedStep g_blueSteps[] = {
    LedStep(LedState::Off, 256),
    LedStep(LedState::FadeIn, 128),
    LedStep(LedState::FadeOut, 128),
    LedStep(LedState::Off, 128),
    LedStep(LedState::FadeIn, 128),
    LedStep(LedState::FadeIn, 128),
    LedStep(LedState::FadeOut, 128),
    LedStep(LedState::Off, 256),
};

LedStep g_offSteps[] = {
    LedStep(LedState::Off, 0)
};

LedInfo g_Leds[] = {
    LedInfo(LedPin::Red, g_redSteps, LEN(g_redSteps)),
    LedInfo(LedPin::Green, g_greenSteps, LEN(g_greenSteps)),
    LedInfo(LedPin::Blue, g_blueSteps, LEN(g_blueSteps)),
    LedInfo(LedPin::Orange, g_offSteps, LEN(g_offSteps)),
    LedInfo(LedPin::White, g_offSteps, LEN(g_offSteps))
};


void processLeds()
{
    for (unsigned i = 0; i < LEN(g_Leds); ++i)
    {
        g_Leds[i].process();
    }
}

void setupLeds()
{
    for (unsigned i = 0; i < LEN(g_Leds); ++i)
    {
        pinMode(g_Leds[i].pin, OUTPUT);
        digitalWrite(g_Leds[i].pin, LOW);
    }
}

void loadLedsConfig()
{
    LedStep x[170];
    Serial.println(sizeof(x));
    Serial.println(freeMemory());
}

void readCommandFromSerial()
{
    if (Serial.available() > 0)
    {
        if (g_CmdParser->parse(Serial) == ParserState_Done)
        {
            const Command* cmd = g_CmdParser->command();
            Serial.print(F("type: "));
            Serial.print(cmd->type);
            Serial.print(F(", argc: "));
            Serial.print(cmd->argc);
            Serial.println(F(", params:"));

            SaveCfgCommand::SaveCfgParam* params =
                reinterpret_cast<SaveCfgCommand::SaveCfgParam*>(cmd->argv);
            for (int i = 0; i < cmd->argc; ++i)
            {
                Serial.print(F("  name: "));
                Serial.print((char)params[i].name);
                Serial.print(F(", effect: "));
                Serial.print((char)params[i].effect);
                Serial.print(F(", ticks: "));
                Serial.print(params[i].ticks);
                Serial.print(F(", extra: "));
                Serial.print((int)params[i].extra[0]);
                Serial.println((int)params[i].extra[1]);
            }
        }
//        Serial.print(F("*** "));
//        Serial.println(freeMemory());
    }
}


Task g_taskSetupLeds(0, 1, &setupLeds);
Task g_taskLoadLedsConfig(0, 1, &loadLedsConfig);
Task g_taskProcessLeds(15, -1, &processLeds);
Task g_taskReadCommand(50, -1, &readCommandFromSerial);
Scheduler g_taskManager;


void setup()
{
    Serial.begin(9600);
    Serial.println(freeMemory());
    g_taskManager.init();
    g_taskManager.addTask(g_taskSetupLeds);
    g_taskManager.addTask(g_taskLoadLedsConfig);
    g_taskManager.addTask(g_taskProcessLeds);
    g_taskManager.addTask(g_taskReadCommand);
    g_taskManager.enableAll();
}

void loop()
{
    g_taskManager.execute();
//    Serial.println(freeMemory());
}
