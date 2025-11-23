#ifndef SYSTEMCONFIGURATION_H
#define SYSTEMCONFIGURATION_H

#include <Arduino.h>
#include <functional>
#include <vector>
#include <ArduinoJson.h>
#include "..\ButtonProcessor\ButtonProcessor.h"
#include "..\DebugUtils\DebugUtils.h"

// Typedef for "ButtonFactory".
// It takes a GPIO pin number and returns a pointer to a GenericButton implementation.
typedef GenericButton* (*ButtonFactory)(int gpioPin);

class SystemConfiguration {
    public:
        static SystemConfiguration* ParseJson(
            const char* jsonString, 
            size_t length,
            ButtonActionProcessor actionProcessor,
            ButtonFactory buttonFactory);

        ~SystemConfiguration();

        ButtonProcessor& getButtonProcessor() { return m_buttonProcessor; }
        String getDisplayConfigurationFile() { return m_displayConfigurationFile; }
        String getBluetoothConfigurationFile() { return m_bluetoothConfigurationFile; }
        String getStyleConfigurationFile() { return m_styleConfigurationFile; }
        float getClockMultiplier() { return m_clockMultiplier; }

        // Additional configs to add:
        // BatteryMonitorConfiguration
        // PowerLedConfiguration
        // Tm1637Configuration

    private:
        ButtonProcessor m_buttonProcessor;
        String m_displayConfigurationFile{"displayconfiguration.json"};
        String m_bluetoothConfigurationFile{"bluetoothconfiguration.json"};
        String m_styleConfigurationFile{"styleconfiguration.json"};
        float m_clockMultiplier{1.0f};
        std::vector<GenericButton*> m_buttons;  // Track buttons for cleanup

        SystemConfiguration() {};
        void configureButtonProcessor(JsonVariant buttonConfigs, ButtonFactory buttonFactory);
        void addButtonDefinitions(JsonArray definitions, ButtonFactory buttonFactory);
        void addButtonActions(JsonArray actions);
        std::vector<String> getStringList(JsonVariant array);
};

#endif