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
    typedef int Type;

    static const Type On = 0;
    static const Type Off = 1;
    static const Type FadeIn = 2;
    static const Type FadeOut = 3;
    static const Type Unused = 4;

#ifdef _DEBUG
    static const char * toString(const LedState::Type value)
    {
        switch(value)
        {
            case LedState::On: return "on";
            case LedState::Off: return "off";
            case LedState::FadeIn: return "fade-in";
            case LedState::FadeOut: return "fade-out";
            case LedState::Unused: return "unused";
        }
        return "undefined";
    }
#endif
};

struct LedStep
{
    LedState::Type m_state;
    int m_ticks;
    byte m_factor;
    bool m_once;

protected:
    LedStep(const LedState::Type state, const int ticks, const byte factor = 0, const bool once = false)
        : m_state(state)
        , m_ticks(ticks)
        , m_factor(factor)
        , m_once(once)
    {}
};

struct LedStepOn : public LedStep
{
    LedStepOn(const int ticks, const bool once = false)
        : LedStep(LedState::On, ticks, 0, once)
    {}
};

struct LedStepOff : public LedStep
{
    LedStepOff(const int ticks, const bool once = false)
        : LedStep(LedState::Off, ticks, 0, once)
    {}
};

struct LedStepFadeIn : public LedStep
{
    LedStepFadeIn(const int ticks, const byte factor, const bool once = false)
        : LedStep(LedState::FadeIn, ticks, factor, once)
    {}
};

struct LedStepFadeOut : public LedStep
{
    LedStepFadeOut(const int ticks, const byte factor, const bool once = false)
        : LedStep(LedState::FadeOut, ticks, factor, once)
    {}
};

struct LedRuntimeInfo
{
    int m_pwm;
    LedState::Type m_state;
    LedState::Type m_lastState;
    int m_ticks;
    int m_step;

    LedRuntimeInfo()
        : m_pwm(0)
        , m_state(LedState::On)
        , m_lastState(LedState::On)
        , m_ticks(0)
        , m_step(0)
    {}
};

struct LedInfo
{
    LedPin::Type m_pin;
    LedStep* m_steps;
    int m_stepsCount;
    LedRuntimeInfo m_runtime;

    LedInfo(const int pin, struct LedStep* steps, const int steps_count)
        : m_pin(pin)
        , m_steps(steps)
        , m_stepsCount(steps_count)
    {
        m_runtime.m_state = m_steps[0].m_state;
        m_runtime.m_ticks = m_steps[0].m_ticks;
    }

    inline int currentStep() const
    {
        return m_runtime.m_step;
    }

    void nextStep()
    {
        do
        {
            m_runtime.m_step = (currentStep() + 1) % m_stepsCount;
        }
        while (m_steps[currentStep()].m_once);
        m_runtime.m_lastState = m_runtime.m_state;
        m_runtime.m_state = m_steps[currentStep()].m_state;
        m_runtime.m_ticks = m_steps[currentStep()].m_ticks;
    }

    bool isNextStep()
    {
        return --m_runtime.m_ticks < 0;
    }

    void process()
    {
        switch (m_runtime.m_state)
        {
        case LedState::On:
            if (m_runtime.m_lastState != LedState::On)
            {
                analogWrite(m_pin, m_runtime.m_pwm);
                m_runtime.m_lastState = LedState::On;
            }
            break;
        case LedState::Off:
            if (m_runtime.m_lastState != LedState::Off)
            {
                analogWrite(m_pin, 0);
                m_runtime.m_lastState = LedState::Off;
            }
            break;
        case LedState::FadeIn:
            if (m_runtime.m_lastState != LedState::FadeIn)
            {
                m_runtime.m_lastState = LedState::FadeIn;
            }
            analogWrite(m_pin, m_runtime.m_pwm);
            m_runtime.m_pwm += m_steps[m_runtime.m_step].m_factor;
            break;
        case LedState::FadeOut:
            if (m_runtime.m_lastState != LedState::FadeOut)
            {
                m_runtime.m_lastState = LedState::FadeOut;
            }
            analogWrite(m_pin, m_runtime.m_pwm);
            m_runtime.m_pwm -= m_steps[m_runtime.m_step].m_factor;
            break;
        case LedState::Unused:
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
        Serial.print("Led '");
        Serial.print(LedPin::toString(this->m_pin));
        Serial.print("' transit from '");
        Serial.print(LedState::toString(this->m_runtime.m_lastState));
        Serial.print("' to '");
        Serial.print(LedState::toString(this->m_runtime.m_state));
        Serial.print("': ticks=");
        Serial.print(this->m_runtime.m_ticks);
        Serial.print(", pwm=");
        Serial.println(this->m_runtime.m_pwm);
    }
#endif
};

LedStep g_greenSteps[] = {
    LedStepFadeIn(128, 1),
    LedStepFadeOut(128, 1),
    LedStepOff(128),
    LedStepFadeIn(128, 1),
    LedStepOn(640),
    LedStepFadeOut(128, 1),
};

LedStep g_redSteps[] = {
    LedStepOff(128),
    LedStepFadeIn(128, 1),
    LedStepFadeOut(128, 1),
    LedStepOff(128),
    LedStepFadeIn(128, 1),
    LedStepOn(384),
    LedStepFadeOut(128, 1),
    LedStepOff(128),
};

LedStep g_blueSteps[] = {
    LedStepOff(256),
    LedStepFadeIn(128, 1),
    LedStepFadeOut(128, 1),
    LedStepOff(128),
    LedStepFadeIn(128, 1),
    LedStepOn(128),
    LedStepFadeOut(128, 1),
    LedStepOff(256),
};

LedStep g_offSteps[] = {
    LedStepOff(0)
};

LedInfo g_Leds[] = {
    LedInfo(LedPin::Green, g_greenSteps, LEN(g_greenSteps)),
    LedInfo(LedPin::Red, g_redSteps, LEN(g_redSteps)),
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
        pinMode(g_Leds[i].m_pin, OUTPUT);
        digitalWrite(g_Leds[i].m_pin, LOW);
    }
}

void readCommandFromSerial()
{
    if (Serial.available() > 0)
    {
        if (g_CmdParser->parse(Serial) == ParserState_Done)
        {
            const Command* cmd = g_CmdParser->command();
            Serial.print("type: ");
            Serial.print(cmd->type);
            Serial.print(", argc: ");
            Serial.print(cmd->argc);
            Serial.println(", params:");
            for (int i = 0; i < cmd->argc; ++i)
            {
                SaveCfgCommand::SaveCfgParam* params =
                    reinterpret_cast<SaveCfgCommand::SaveCfgParam*>(cmd->argv);
                Serial.print("  name: ");
                Serial.print((char)params[i].name);
                Serial.print(", effect: ");
                Serial.print((char)params[i].effect);
                Serial.print(", ticks: ");
                Serial.print(params[i].ticks);
                Serial.print(", extra: ");
                Serial.print((int)params[i].extra[0]);
                Serial.println((int)params[i].extra[1]);
            }
        }
        Serial.print("*** ");
        Serial.println(freeMemory());
    }
}


Task g_taskSetupLeds(0, 1, &setupLeds);
Task g_taskProcessLeds(15, -1, &processLeds);
Task g_taskReadCommand(50, -1, &readCommandFromSerial);
Scheduler g_taskManager;


void setup()
{
    Serial.begin(9600);
    g_taskManager.init();
    g_taskManager.addTask(g_taskSetupLeds);
    g_taskManager.addTask(g_taskProcessLeds);
    g_taskManager.addTask(g_taskReadCommand);
    g_taskManager.enableAll();
}

void loop()
{
    g_taskManager.execute();
}
