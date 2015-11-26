//#define _LED_PROCESSOR_DEBUG
#include <Arduino.h>
#include <Illume_LedProcessor.h>


/**
 * Global
 */
LedAnimationStep g_DefaultStep(LedAnimation_Off, 0);

/**
 * Print helpers
 */
String LedNameToString(const LedName value)
{
    switch(value)
    {
    case LedName_Green: return String(F("green"));
    case LedName_Yellow: return String(F("yellow"));
    case LedName_Blue: return String(F("blue"));
    case LedName_White: return String(F("white"));
    case LedName_Red: return String(F("red"));
    case LedName_Any: return String(F("any"));
    }
    return String(F("undefined"));
}

String LedAnimationToString(const LedAnimation value)
{
    switch(value)
    {
    case LedAnimation_On: return String(F("on"));
    case LedAnimation_Off: return String(F("off"));
    case LedAnimation_FadeIn: return String(F("fade-in"));
    case LedAnimation_FadeOut: return String(F("fade-out"));
    }
    return String(F("undefined"));
}

String LedAnimationFactorToString(const LedAnimationFactor value)
{
    switch(value)
    {
    case LedAnimationFactor_Quarter: return String(F("quarter"));
    case LedAnimationFactor_Half: return String(F("half"));
    case LedAnimationFactor_Once: return String(F("once"));
    case LedAnimationFactor_Twice: return String(F("twice"));
    }
    return String(F("undefined"));
}

/**
 * @brief Class intended to process animation sequences
 */
class SingleProcessor : public LedProcessor
{
private:
    struct LedRuntimeInfo
    {
        float pwm;
        LedAnimation currAnim;
        LedAnimation prevAnim;
        int duration;
        int step;

        LedRuntimeInfo()
            : pwm(.0)
            , currAnim(LedAnimation_Off)
            , prevAnim(LedAnimation_Off)
            , duration(0)
            , step(0)
        {}
    };

    const int m_pin;
    const LedName m_ledName;
    const LedAnimationStep* m_steps;
    byte m_stepsCount;
    LedRuntimeInfo m_runtime;

public:
    SingleProcessor(const int pin, const LedName led_name)
        : m_pin(pin)
        , m_ledName(led_name)
        , m_steps(NULL)
        , m_stepsCount(0)
    {
    }

    virtual void apply(const LedAnimationStep* steps, const byte stepsCount)
    {
        m_steps = steps;
        m_stepsCount = stepsCount;
        m_runtime.step = 0;
        m_runtime.pwm = .0;
        m_runtime.currAnim = currentStep()->info.animation;
        m_runtime.duration = currentStep()->duration;
#ifdef _LED_PROCESSOR_DEBUG
        debug();
#endif
    }

    virtual void init()
    {
        pinMode(m_pin, OUTPUT);
        digitalWrite(m_pin, LOW);
    }

    virtual void process()
    {
        switch (m_runtime.currAnim)
        {
        case LedAnimation_On:
            if (m_runtime.prevAnim != LedAnimation_On)
            {
                m_runtime.pwm = m_runtime.pwm == 0 ? 255 : m_runtime.pwm;
                analogWrite(m_pin, m_runtime.pwm);
                m_runtime.prevAnim = LedAnimation_On;
            }
            break;
        case LedAnimation_Off:
            if (m_runtime.prevAnim != LedAnimation_Off)
            {
                analogWrite(m_pin, 0);
                m_runtime.prevAnim = LedAnimation_Off;
            }
            break;
        case LedAnimation_FadeIn:
            if (m_runtime.prevAnim != LedAnimation_FadeIn)
            {
                m_runtime.prevAnim = LedAnimation_FadeIn;
            }
            switch (currentStep()->info.factor)
            {
            case LedAnimationFactor_Quarter: m_runtime.pwm += 0.25; break;
            case LedAnimationFactor_Half: m_runtime.pwm += 0.5; break;
            case LedAnimationFactor_Once: m_runtime.pwm += 1.0; break;
            case LedAnimationFactor_Twice: m_runtime.pwm += 2.0; break;
            }
            analogWrite(m_pin, (int)m_runtime.pwm);
            break;
        case LedAnimation_FadeOut:
            if (m_runtime.prevAnim != LedAnimation_FadeOut)
            {
                m_runtime.prevAnim = LedAnimation_FadeOut;
            }
            switch (currentStep()->info.factor)
            {
            case LedAnimationFactor_Quarter: m_runtime.pwm -= 0.25; break;
            case LedAnimationFactor_Half: m_runtime.pwm -= 0.5; break;
            case LedAnimationFactor_Once: m_runtime.pwm -= 1.0; break;
            case LedAnimationFactor_Twice: m_runtime.pwm -= 2.0; break;
            }
            analogWrite(m_pin, (int)m_runtime.pwm);
            break;
        }
        if (isNextStep())
        {
            nextStep();
#ifdef _LED_PROCESSOR_DEBUG
            if (m_runtime.prevAnim != m_runtime.currAnim)
            {
                debug();
            }
#endif
        }
    }

private:
    inline bool stepIndexValid() const
    {
        return m_runtime.step >= 0 && m_runtime.step < getStepsCount();
    }

    inline const LedAnimationStep * const getStep(const byte index) const
    {
        return (m_steps != NULL && stepIndexValid()) ? &m_steps[index] : NULL;
    }

    inline byte getStepsCount() const
    {
        return m_stepsCount;
    }

    inline bool thisLedStep(const LedAnimationStep * const step) const
    {
        return step->info.name == m_ledName || step->info.name == LedName_Any;
    }

    inline const LedAnimationStep * const currentStep() const
    {
        const LedAnimationStep * const result = getStep(m_runtime.step);
        if (result == NULL || !thisLedStep(result))
        {
            return &g_DefaultStep;
        }
        return result;
    }

    void advanceNextStep()
    {
        const byte initialStepIndex = m_runtime.step;
        const LedAnimationStep * step = NULL;
        do
        {
            m_runtime.step = (m_runtime.step + 1) % getStepsCount();
            step = currentStep();
        }
        while ((!thisLedStep(step) || onceStep(step)) && m_runtime.step != initialStepIndex);
    }

    inline void nextStep()
    {
        advanceNextStep();
        m_runtime.prevAnim = m_runtime.currAnim;
        m_runtime.currAnim = currentStep()->info.animation;
        m_runtime.duration = currentStep()->duration;
    }

    inline bool isNextStep()
    {
        return --m_runtime.duration <= 0;
    }

    inline bool onceStep(const LedAnimationStep * const step) const
    {
        return step->info.once;
    }

    void debug() const
    {
        // [step_idx:led_name] prev to curr: pwm
        Serial.print(F("["));
        Serial.print(m_runtime.step);
        Serial.print(F(","));
        Serial.print(LedNameToString(currentStep()->info.name));
        Serial.print(F("]: transit from '"));
        Serial.print(LedAnimationToString(m_runtime.prevAnim));
        Serial.print(F("' to '"));
        Serial.print(LedAnimationToString(m_runtime.currAnim));
        Serial.print(F("': duration="));
        Serial.print(m_runtime.duration);
        Serial.print(F(", pwm="));
        Serial.println(m_runtime.pwm);
    }
};

LedProcessor* createLedProcessor(const int pin, const LedName led_name)
{
    return new SingleProcessor(pin, led_name);
}
