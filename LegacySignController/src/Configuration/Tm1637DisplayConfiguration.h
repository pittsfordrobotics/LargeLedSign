#ifndef TM1637DISPLAYCONFIGURATION_H
#define TM1637DISPLAYCONFIGURATION_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "Utils\JsonUtils.h"

class Tm1637DisplayConfiguration {
    public:
        static Tm1637DisplayConfiguration fromJson(JsonVariant displayConfig);
        Tm1637DisplayConfiguration();
        Tm1637DisplayConfiguration(const Tm1637DisplayConfiguration& other);

        bool isEnabled() { return m_enabled; }
        int getClockGpioPin() { return m_clockGpioPin; }
        int getDataGpioPin() { return m_dataGpioPin; }
        int getBrightness() { return m_brightness; }

        Tm1637DisplayConfiguration& operator=(const Tm1637DisplayConfiguration& other);

    private:
        void copy(const Tm1637DisplayConfiguration& other);

        bool m_enabled{false};
        int m_clockGpioPin{0};
        int m_dataGpioPin{1};
        byte m_brightness{5}; // Brightness level from 0 (min) to 7 (max)
};

#endif