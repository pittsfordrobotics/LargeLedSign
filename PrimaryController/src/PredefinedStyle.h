#ifndef PREDEFINEDSTYLE_H
#define PREDEFINEDSTYLE_H

#define DEFAULT_BRIGHTNESS 10

#include <Arduino.h>
#include <PatternData.h>
#include <vector>

enum class PredefinedStyles {
    Pink_Solid,
    RedPink_Right,
    BluePink_Right,
    Rainbow_Right,
    RedPink_Random,
    BluePink_Random,
    Rainbow_Random,
    RedPink_CenterOut,
    BluePink_CenterOut
};

class PredefinedStyle {
    public:
        static PredefinedStyle getPredefinedStyle(PredefinedStyles styleName);
        PredefinedStyle(String name, byte brightness, byte speed, PatternData patternData);
        PredefinedStyle(const PredefinedStyle& other);
        PredefinedStyle& operator=(const PredefinedStyle& other);
        String getName();
        PatternData getPatternData();
        byte getBrightness();
        byte getSpeed();

    private:
        String m_name;
        byte m_brightness{0};
        byte m_speed{0};
        PatternData m_patternData;
        
        static ulong color(byte red, byte green, byte blue);
};

#endif