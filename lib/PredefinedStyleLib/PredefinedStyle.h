#ifndef PREDEFINEDSTYLE_H
#define PREDEFINEDSTYLE_H

#include <Arduino.h>
#include <PatternData.h>
#include <vector>
#include "PredefinedStyles.h"

class PredefinedStyle {
    public:
        static PredefinedStyle getPredefinedStyle(PredefinedStyles styleName);
        PredefinedStyle(String name, byte speed, PatternData patternData);
        PredefinedStyle(const PredefinedStyle& other);
        PredefinedStyle& operator=(const PredefinedStyle& other);
        String getName();
        PatternData getPatternData();
        byte getSpeed();

    private:
        String m_name;
        byte m_speed{0};
        PatternData m_patternData;
        
        static ulong color(byte red, byte green, byte blue);
};

#endif