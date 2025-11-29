#include <Arduino.h>
#include <ArduinoBLE.h>
#include <SD.h>
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

/*
Valid GPIO pins for SPI0:
CLK: 2, 6, 18, 22
CIPO: 0, 4, 16, 20
COPI: 3, 7, 19, 23
 */
#define SDCARD_CHIPSELECT  18
#define SDCARD_SPI_CLOCK  6
#define SDCARD_SPI_COPI   7
#define SDCARD_SPI_CIPO    4
// The legacy sign uses the default COPI pin for the Status Display CLK signal.
#define SDCARD_ALT_CHIPSELECT  20
#define SDCARD_ALT_SPI_CLOCK  18
#define SDCARD_ALT_SPI_COPI   19
#define SDCARD_ALT_SPI_CIPO    4

// Function prototypes
SystemConfiguration* readSystemConfiguration();
StatusDisplay* createStatusDisplay(Tm1637DisplayConfiguration& config);
NeoPixelDisplay* createNeoPixelDisplay(String displayConfigFile);
StyleConfiguration* readStyleConfiguration(String styleConfigFile);
String autogenerateSystemConfigurationJson();
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
const char* getSdFileContents(String filename);

const char* defaultSystemConfigJson = R"json(
    {
        "displayConfigurationFile": "::[[DISPLAYTYPENAME]]::",
        "styleConfigurationFile": "::[[STYLECONFIGTYPENAME]]::",
        "bluetooth": {
            "enabled": true,
            "uuid": "[[BT_UUID]]",
            "localName": "[[BT_LOCALNAME]]"
        },
        "buttons": {[[BUTTONS]]},
        "batteryMonitor": {
            "enabled": true,
            "analogInputGpioPin": 29,
            "inputMultiplier": 4.83,
            "voltageToEnterLowPowerState": 6.7,
            "voltageToExitLowPowerState": 7.2
        }
    }
)json";

const char* legacyButtonDefinitionJson = R"json(
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
    }
)json";