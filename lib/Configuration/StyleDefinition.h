#ifndef STYLEDEFINITION_H
#define STYLEDEFINITION_H

#include <Arduino.h>
#include <DisplayPatternLib.h>

class StyleDefinition
{
    public:
        StyleDefinition();
        StyleDefinition(PatternData patternData, byte speed);
        StyleDefinition(const StyleDefinition& other);

        PatternData getPatternData() { return m_patternData; };
        void setPatternData(PatternData patternData) { m_patternData = patternData; };
        byte getSpeed() { return m_speed; };
        void setSpeed(byte speed) { m_speed = speed; };
        String getName() { return m_name; };
        void setName(String name) { m_name = name; };

		StyleDefinition& operator=(const StyleDefinition& other);
		bool operator==(const StyleDefinition& other);
		bool operator!=(const StyleDefinition& other);

    private:
        PatternData m_patternData;
        byte m_speed;
        String m_name{""};
};

#endif