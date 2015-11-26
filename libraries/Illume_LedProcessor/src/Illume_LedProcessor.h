#ifndef __ILLUME_LED_PROCESSOR_H__
#define __ILLUME_LED_PROCESSOR_H__

#include <Arduino.h>

enum LedName
{
    LedName_None = 0,
    LedName_Red = 0,
    LedName_Green,
    LedName_Blue,
    LedName_Yellow,
    LedName_White,
    LedName_Any
};
String LedNameToString(const LedName value);

enum LedAnimation
{
    LedAnimation_None = 0,
    LedAnimation_On = 0,
    LedAnimation_Off,
    LedAnimation_FadeIn,
    LedAnimation_FadeOut
};
String LedAnimationToString(const LedAnimation value);

enum LedAnimationFactor
{
    LedAnimationFactor_None = 0,
    LedAnimationFactor_Quarter = 0,
    LedAnimationFactor_Half,
    LedAnimationFactor_Once,
    LedAnimationFactor_Twice
};
String LedAnimationFactorToString(const LedAnimationFactor value);

/*
 * @brief Represents single animation step for specific LED. Single step include
 * the following information: LED name, animation type, animation factor, whether
 * this step should be executed once or not, step duration in ticks.
 */
struct LedAnimationStep
{
    struct
    {
        LedName name:3;
        LedAnimation animation:2;
        LedAnimationFactor factor:2;
        bool once:1;
    } info;
    int duration;

    LedAnimationStep()
        : duration(0)
    {
        info.name = LedName_None;
        info.animation = LedAnimation_None;
        info.factor = LedAnimationFactor_None;
        info.once = 0;
    }

    LedAnimationStep(
        const LedAnimation animation
        , const int duration
        , const LedAnimationFactor factor = LedAnimationFactor_Once
        , const bool once = false)
        : duration(duration)
    {
        info.name = LedName_Any;
        info.animation = animation;
        info.factor = factor;
        info.once = once;
    }
};

/*
 * @brief Class intended to process single LED animation sequence using @ref process
 * method. Prior to usage @ref init should be called.
 */
class LedProcessor
{
public:
    virtual ~LedProcessor() {}

    /*
     * @brief Initializes LED processor
     */
    virtual void init() = 0;

    /*
     * @brief Process single step of LED animation sequence
     */
    virtual void process() = 0;

    /*
     * @brief Applies new LED animation sequence
     * @param steps List of new animation steps
     * @param steps_cout number of entries in the @ref steps
     */
    virtual void apply(const LedAnimationStep* steps, const byte steps_count) = 0;
};

LedProcessor* createLedProcessor(const int pin, const LedName led_name);

#endif // __ILLUME_LED_PROCESSOR_H__
