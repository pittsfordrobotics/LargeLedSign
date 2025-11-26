#include <Arduino.h>
#include <ArduinoBLE.h>
#include <vector>
#include <algorithm>
#include <BluetoothCommon.h>
#include <DisplayPatternLib.h>
#include <StatusDisplayLib.h>
#include "ArduinoPushButton/ArduinoPushButton.h"
#include <ButtonProcessor.h>
#include <Configuration.h>
#include "NeoPixelDisplay.h"

#define TELEMETRYINTERVAL 2000     // Interval (msec) for updating the telemetry.

#define LOW_BRIGHTNESS_PIN  12  // When this GPIO pin # is pulled low, the default brightness will be much lower than normal.

// Function prototypes
SystemConfiguration* readSystemConfiguration();
StatusDisplay* createStatusDisplay(Tm1637DisplayConfiguration& config);
NeoPixelDisplay* createNeoPixelDisplay(String displayConfigFile);
StyleConfiguration* readStyleConfiguration(String styleConfigFile);
void initializeBatteryMonitor(BatteryMonitorConfiguration& config);
void initializeDefaultStyleProperties(StyleDefinition& defaultStyleDefinition);
void initializeBLEService(BluetoothConfiguration& config);
void configurePowerLed(PowerLedConfiguration& config);
void readSettingsFromBLE();
void setManualStyle(StyleDefinition styleDefinition);
void updateTelemetry();
void updateLedTelemetry();
float getCalculatedBatteryVoltage();
int getRawVoltageInputLevel();
void displayBatteryVoltage();
void checkForLowPowerState();
void updateLEDs();
void processButtonAction(int callerId, String actionName, std::vector<String> arguments);

// Default system configuration for the legacy sign.
// Should eventually read from a configuration file.
const char* SystemConfigurationFileContents = R"json(
{
    "comment": "Legacy sign",
    "displayConfigurationFile": "displayconfiguration.json",
    "styleConfigurationFile": "styleconfigurations.json",
    "bluetooth": {
        "enabled": true,
        "uuid": "99be4fac-c708-41e5-a149-74047f554cc1",
        "localName": "Lucia's Sign"
    },
    "buttons": {
        "definitions": [
            {
                "id": "1",
                "enabled": true,
                "gpioPin": 27
            },
            {
                "id": "2",
                "enabled": true,
                "gpioPin": 28
            },
            {
                "id": "3",
                "enabled": true,
                "gpioPin": 26
            }
        ],
        "actions": [
            {
                "buttonIds": ["1"],
                "tapAction": "changeStyle",
                "tapActionArguments": ["Pink", "RainbowRandom"],
                "longTapAction": "changeStyle",
                "longTapActionArguments": ["Fire"]
            },
            {
                "buttonIds": ["2"],
                "tapAction": "changeStyle",
                "tapActionArguments": ["BluePinkRandom", "BluePinkDigit"],
                "longTapAction": "batteryVoltage",
                "longTapActionArguments": []
            },
            {
                "buttonIds": ["3"],
                "tapAction": "changeStyle",
                "tapActionArguments": ["RedPinkRandom", "RedPinkDigit"],
                "longTapAction": "disconnectBT",
                "longTapActionArguments": []
            }
        ]
    },
    "batteryMonitor": {
        "enabled": true,
        "analogInputGpioPin": 29,
        "inputMultiplier": 4.83,
        "voltageToEnterLowPowerState": 6.7,
        "voltageToExitLowPowerState": 7.2
    },
    "powerLed": {
        "enabled": false,
        "gpioPin": 22
    },
    "clockMultiplier": 1.0,
    "tm1637Display": {
        "enabled": true,
        "clockGpioPin": 7,
        "dataGpioPin": 5,
        "brightness": 5
    }
}
)json";