#include <Arduino.h>
#include <ArduinoBLE.h>
#include <SD.h>
#include <vector>
#include <algorithm>
#include "Bluetooth\SecondaryPeripheral.h"
#include "Bluetooth\SecondaryClient.h"
#include <DisplayPatternLib.h>
#include "StatusDisplay\StatusDisplay.h"
#include "StatusDisplay\NullStatusDisplay.h"
#include "StatusDisplay\TM1637StatusDisplay.h"
#include "ArduinoPushButton/ArduinoPushButton.h"
#include <ButtonProcessor.h>
#include <Configuration.h>
#include "NeoPixelDisplay.h"

#define INITIAL_DELAY 2000  // Initial delay to allow serial monitor to connect.
#define TELEMETRYINTERVAL 2000     // Interval (msec) for updating the telemetry.
#define TEST_MODE_PIN 13       // The GPIO pin # that is used to put the controller into test mode. (Low brightness, and use TestMatrix display.)

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

#define ORDER_SELECTOR_PINS  20, 21, 5 // The set of GPIO pin #s that tell the controller what position the sign should be in (MSB to LSB).
#define STYLE_TYPE_SELECTOR_PINS 16, 17, 18, 19  // The set of GPIO pin #s that tell the controller what style (digit # or logo) the sign should be (MSB to LSB).

#define MAX_SECONDARY_SCAN_TIME 2000  // The amount of time (msec) to wait for a connection to a secondary peripheral.
#define MAX_TOTAL_SCAN_TIME 10000     // The total time (msec) to spend looking for secondary peripherals.
#define SECONDARY_PING_INTERVAL 1500  // The interval (msec) at which to ping secondaries to verify connection.

// Function prototypes
SystemConfiguration* readSystemConfiguration();
StatusDisplay* createStatusDisplay(Tm1637DisplayConfiguration& config);
std::vector<NeoPixelDisplay*>* createNeoPixelDisplays(String displayConfigFile);
StyleConfiguration* readStyleConfiguration(String styleConfigFile);
void initializeBatteryMonitor();
void initializeDefaultStyleProperties(StyleDefinition& defaultStyleDefinition);
void startBleService();
void initializeBlePeripheralService();
void initializeProxyService();
void initializePowerLed();
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
byte getSignType();
byte getSignPosition();
const char* readBuiltInFile(String filename);
const char* copyString(const char* source, size_t length);
void populateSecondaryClients();
SecondaryClient* scanForSecondaryClient();
void updateOffsetDataForSecondaryClients();
void updateAllSecondaries();
void checkSecondaryConnections();
void resetSecondaryConnections();

const char* defaultSystemConfigJsonForSecondaries = R"json(
    {
        "displayConfigurationFile": "::Display[[SIGNTYPE1]]::",
        "styleConfigurationFile": "::[[STYLECONFIGTYPENAME]]::",
        "bluetooth": {
            "enabled": true,
            "isSecondaryModeEnabled": true,
            "uuid": "1221ca8d-4172-4946-bcd1-f9e4b40ba6b0",
            "localName": "3181 LED Controller [[SIGNPOSITION]]-[[SIGNTYPE2]]"
        },
        "buttons": {[[BUTTONS]]},
        "batteryMonitor": {
            "enabled": true,
            "analogInputGpioPin": 26,
            "inputMultiplier": 4.83,
            "voltageToEnterLowPowerState": 6.7,
            "voltageToExitLowPowerState": 7.2
        }
    }
)json";

// Not needed ???  We should assume we can read the config from the SD card.
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

const char* defaultPrimaryButtonDefinitionJson = R"json(
        "definitions": [
            {
                "id": "1",
                "enabled": true,
                "gpioPin": 27
            },
            {
                "id": "2",
                "enabled": true,
                "gpioPin": 21
            },
            {
                "id": "3",
                "enabled": true,
                "gpioPin": 26
            },
            {
                "id": "4",
                "enabled": true,
                "gpioPin": 5
            }
        ],
        "actions": [
            {
                "buttonIds": ["1"],
                "tapAction": "changeStyle",
                "tapActionArguments": ["Pink"],
                "longTapAction": "secondaryBatteryVoltage"
            },
            {
                "buttonIds": ["2"],
                "tapAction": "changeStyle",
                "tapActionArguments": ["BluePinkRandom_Large", "BluePinkDigit"]
            },
            {
                "buttonIds": ["3"],
                "tapAction": "changeStyle",
                "tapActionArguments": ["RedPinkRandom_Large", "RedPinkDigit"]
            },
            {
                "buttonIds": ["4"],
                "tapAction": "changeStyle",
                "tapActionArguments": ["RainbowRandom_Large", "RainbowRight"],
                "longTapAction": "disconnectBT",
                "longTapActionArguments": []
            },
            {
                "buttonIds": ["3", "4"],
                "longTapAction": "resetSecondaries"
            }
        ]
)json";