#ifndef SERVICESTATUS_H
#define SERVICESTATUS_H

#include <Arduino.h>
#include <vector>
#include "StringUtils.h"

class ServiceStatus {
    public:
        String getStyleNames() { return m_styleNames; }
        String getPatternNames() { return m_patternNames; }
        byte getStyle() { return m_style; }
        byte getPattern() { return m_pattern; }
        byte getBrightness() { return m_brightness; }
        byte getStep() { return m_step; }
        byte getSpeed() { return m_speed; }
        String getSignData() { return m_signData; }

        int getSignType() { return m_signType; }
        int getSignOrder() { return m_signOrder; }
        int getColumnCount() { return m_columnCount; }
        int getPixelCount() { return m_pixelCount; }

        void setStyleNames(String styleNames) { m_styleNames = styleNames; }
        void setPatternNames(String patternNames) { m_patternNames = patternNames; }
        void setStyle(byte style) { m_style = style; }
        void setPattern(byte pattern) { m_pattern = pattern; }
        void setBrightness(byte brightness) { m_brightness = brightness; }
        void setStep(byte step) { m_step = step; }
        void setSpeed(byte speed) { m_speed = speed; }
        void setSignData(String signData);

        ServiceStatus();
        ServiceStatus(const ServiceStatus& other);
        ServiceStatus& operator=(const ServiceStatus& other);
        bool operator==(const ServiceStatus& other);

    private:
        void copy(const ServiceStatus& other);

        String m_styleNames{""};
        String m_patternNames{""};
        byte m_style{0};
        byte m_pattern{0};
        byte m_brightness{0};
        byte m_step{0};
        byte m_speed{0};
        String m_signData{""};
        int m_signType{-1};
        int m_signOrder{-1};
        int m_columnCount{-1};
        int m_pixelCount{-1};
};

#endif