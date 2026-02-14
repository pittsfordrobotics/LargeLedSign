#ifndef SYSTEMCONFIGURATION_H
#define SYSTEMCONFIGURATION_H

#include <Arduino.h>
#include <functional>
#include <vector>
#include <ArduinoJson.h>
#include "BatteryMonitorConfiguration.h"
#include "PowerLedConfiguration.h"
#include "Tm1637DisplayConfiguration.h"
#include "BluetoothConfiguration.h"
#include "ButtonProcessor\ButtonProcessor.h"
#include "Utils\JsonUtils.h"
#include "Utils\DebugUtils.h"

// Typedef for "ButtonFactory".
// It takes a GPIO pin number and returns a pointer to a GenericButton implementation.
typedef GenericButton* (*ButtonFactory)(int gpioPin);

class SystemConfiguration {
    public:
        static SystemConfiguration* ParseJson(
            const char* jsonString, 
            size_t length,
            ButtonFactory buttonFactory);

        ~SystemConfiguration();

        bool isValid() { return m_isValid; }
        ButtonProcessor* getButtonProcessor() { return m_buttonProcessor; }
        String getDisplayConfigurationFile() { return m_displayConfigurationFile; }
        String getStyleConfigurationFile() { return m_styleConfigurationFile; }
        float getClockMultiplier() { return m_clockMultiplier; }

        std::vector<String>& getPublishedColorPatterns() { return m_publishedColorPatterns; }
        std::vector<String>& getPublishedDisplayPatterns() { return m_publishedDisplayPatterns; }

        PowerLedConfiguration& getPowerLedConfiguration() { return m_powerLedConfiguration; }
        BatteryMonitorConfiguration& getBatteryMonitorConfiguration() { return m_batteryMonitorConfiguration; }
        Tm1637DisplayConfiguration& getTm1637DisplayConfiguration() { return m_tm1637DisplayConfiguration; }
        BluetoothConfiguration& getBluetoothConfiguration() { return m_bluetoothConfiguration; }

    private:
        bool m_isValid{false};
        ButtonProcessor* m_buttonProcessor{nullptr};
        String m_displayConfigurationFile{"displayconfiguration.json"};
        String m_styleConfigurationFile{"styleconfiguration.json"};
        float m_clockMultiplier{1.0f};
        std::vector<GenericButton*> m_buttons;  // Track buttons for cleanup
        PowerLedConfiguration m_powerLedConfiguration;
        BatteryMonitorConfiguration m_batteryMonitorConfiguration;
        Tm1637DisplayConfiguration m_tm1637DisplayConfiguration;
        BluetoothConfiguration m_bluetoothConfiguration;
        std::vector<String> m_publishedColorPatterns;
        std::vector<String> m_publishedDisplayPatterns;

        SystemConfiguration() {};
        void configureButtonProcessor(JsonVariant buttonConfigs, ButtonFactory buttonFactory);
        void addButtonDefinitions(JsonArray definitions, ButtonFactory buttonFactory);
        void addButtonActions(JsonArray actions);
        std::vector<String> getStringList(JsonVariant array);
        PowerLedConfiguration parsePowerLedConfiguration(JsonVariant plcVariant);
};

#endif