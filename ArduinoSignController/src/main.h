#include <Arduino.h>
#include <ArduinoBLE.h>
#include <SD.h>
#include <vector>
#include <algorithm>
#include "Bluetooth\SecondaryPeripheral.h"
#include "Bluetooth\SecondaryClient.h"
#include "IO\BatteryMonitor.h"
#include "IO\HardwareSignConfig.h"
#include "IO\FileReader.h"
#include "Patterns\PatternFactory.h"
#include "StatusDisplay\NullStatusDisplay.h"
#include "StatusDisplay\TM1637StatusDisplay.h"
#include "ArduinoPushButton/ArduinoPushButton.h"
#include "ButtonProcessor\ButtonProcessor.h"
#include "Configuration\Configuration.h"
#include "NeoPixelDisplay.h"
#include "Utils\StringUtils.h"

#define INITIAL_DELAY 0000  // Initial delay to allow serial monitor to connect.
#define TELEMETRYINTERVAL 3000     // Interval (msec) for updating the telemetry.

#define MAX_SECONDARY_COUNT 5    // The maximum number of secondary peripherals to connect to in proxy mode.
#define MAX_SECONDARY_SCAN_TIME 2000  // The amount of time (msec) to wait for a connection to a secondary peripheral.
#define MAX_TOTAL_SCAN_TIME 10000     // The total time (msec) to spend looking for secondary peripherals.
#define SECONDARY_PING_INTERVAL 3000  // The interval (msec) at which to ping secondaries to verify connection.

#define LOW_POWER_DURATION 5000  // The amount of time that must elapse before exiting low power mode.

// Function prototypes
SystemConfiguration* readSystemConfiguration();
SystemConfiguration* generateSystemConfigFromHardware();
StatusDisplay* createStatusDisplay(Tm1637DisplayConfiguration& config);
std::vector<NeoPixelDisplay*>* createNeoPixelDisplays(String displayConfigFile);
StyleConfiguration* readStyleConfiguration(String styleConfigFile);
void initializeDefaultStyleProperties(StyleDefinition& defaultStyleDefinition);
void startBleService();
void initializeBlePeripheralService();
void initializeProxyService();
void initializePowerLed();
void readSettingsFromBLE();
void setManualStyle(StyleDefinition styleDefinition);
void updateTelemetry();
void updateLedTelemetry();
void displayBatteryVoltage();
void checkForLowPowerState();
void updateLEDs();
void processButtonAction(int callerId, String actionName, std::vector<String> arguments);
void populateSecondaryClients();
SecondaryClient* scanForSecondaryClient();
void updateOffsetDataForSecondaryClients();
void updateAllSecondaries();
void checkSecondaryConnections();
void disconnectSecondaries();
void resetSecondaryConnections();

// Default values
std::vector<String> DefaultColorPatterns {"SingleColor","TwoColor","Rainbow", "BackgroundPlusThree"};
std::vector<String> DefaultDisplayPatterns {"Solid","Right","Digit","Random","Line","Fire","Spotlight","Radial"};

// JSON configs for signs that can't read from the SD card.
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
