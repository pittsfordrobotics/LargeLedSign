#include "Arduino.h"
#include "BLETypedCharacteristics.h"
#include <ArduinoBLE.h>
#include <vector>
#include <algorithm>
#include "BluetoothCommon.h"
#include "CommonPeripheral.h"
#include "Bluetooth/SecondaryClient.h"
#include <SignConfigurationData.h>
#include <SignOffsetData.h>
#include <SignStatus.h>
#include "Buttons/ManualButton.h"
#include "Display/StatusDisplay.h"

#define NUMBER_OF_SECONDARIES 3

#define TM1637_CLOCK 8    // Digital pin # for the TM1637 clock line
#define TM1637_DIO 7      // Digital pin # for the TM1637 data line
#define TM1637_BRIGHTNESS 5  // Brightness of the TM1637, between 0 and 7

#define CONNECTION_CHECK_INTERVAL 1500   // Number of milliseconds between checks to see if the secondaries are connected.
#define MANUAL_INPUT_PINS 15, 9, 14, 10  // Digital pin #s for the manual input buttons.

#define TELEMETRYINTERVAL 2000     // Interval (msec) for updating the telemetry.
#define MAX_SCAN_TIME 2000         // The amount of time (msec) to wait for a connection to a secondary peripheral.
#define MAX_TOTAL_SCAN_TIME 10000  // The total time (msec) to spend looking for secondary peripherals.

// Function prototypes
void initializeIO();
void populateSecondaries();
void startBLEService();
void readSettingsFromBLE();
void processManualInputs();
void updateInputButtons();
void setManualStyle(uint style);
SecondaryClient* scanForSecondary();
void consolidateTotalsAndWriteToSecondaries();
void checkSecondaryConnections();
void resetSecondaryConnections();
void updateAllSecondaries();
void updateTelemetry();
void displayBatteryVoltages();
ulong color(byte red, byte green, byte blue);