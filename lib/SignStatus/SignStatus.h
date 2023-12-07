#ifndef SIGNSTATUS_H
#define SIGNSTATUS_H

#include <Arduino.h>
#include <vector>
#include <StringUtils.h>
#include <SignConfigurationData.h>
#include <PatternData.h>

class SignStatus {
    public:
        String getStyleNames() { return m_styleNames; }
        String getPatternNames() { return m_patternNames; }
        byte getStyle() { return m_style; }
        byte getPattern() { return m_pattern; }
        byte getBrightness() { return m_brightness; }
        byte getStep() { return m_step; }
        byte getSpeed() { return m_speed; }
        SignConfigurationData getSignConfigurationData() { return m_signConfigData; }
        PatternData getPatternData() { return m_patternData; }

        void setStyleNames(String styleNames) { m_styleNames = styleNames; }
        void setPatternNames(String patternNames) { m_patternNames = patternNames; }
        void setStyle(byte style) { m_style = style; }
        void setPattern(byte pattern) { m_pattern = pattern; }
        void setBrightness(byte brightness) { m_brightness = brightness; }
        void setStep(byte step) { m_step = step; }
        void setSpeed(byte speed) { m_speed = speed; }
        void setSignConfigurationData(SignConfigurationData signConfigData) { m_signConfigData = signConfigData; }
        void setPatternData(PatternData patternData) { m_patternData = patternData; }

        SignStatus();
        SignStatus(const SignStatus& other);
        SignStatus& operator=(const SignStatus& other);
        bool operator==(const SignStatus& other);
        bool operator!=(const SignStatus& other);

    private:
        void copy(const SignStatus& other);

        String m_styleNames{""};
        String m_patternNames{""};
        byte m_style{0};
        byte m_pattern{0};
        byte m_brightness{0};
        byte m_step{0};
        byte m_speed{0};
        SignConfigurationData m_signConfigData;
        PatternData m_patternData;
};

#endif