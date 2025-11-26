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

#define SDCARD_CHIPSELECT  20  // GPIO pin # for the SD card chip select line.
#define SDCARD_SPI_CLOCK  18  // GPIO pin # for the SD card SPI clock line.
#define SDCARD_SPI_COPI   19  // GPIO pin # for the SD card SPI COPI line.
#define SDCARD_SPI_CIPO    4  // GPIO pin # for the SD card SPI CIPO line.

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
const char* getSdFileContents(String filename);
