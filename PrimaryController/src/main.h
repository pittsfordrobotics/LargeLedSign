#include "Arduino.h"
#include "BLETypedCharacteristics.h"
#include <ArduinoBLE.h>
#include <vector>
#include <algorithm>
#include "BluetoothCommon.h"
#include "CommonPeripheral.h"
#include "Bluetooth/SecondaryClient.h"
#include <TM1637Display.h>
#include <SignConfigurationData.h>
#include <SignOffsetData.h>

#define TM1637_CLOCK 8    // Digital pin # for the TM1637 clock line
#define TM1637_DIO 7      // Digital pin # for the TM1637 data line
#define TM1637_BRIGHTNESS 5  // Brightness of the TM1637, between 0 and 7

#define CONNECTION_CHECK_INTERVAL 1500 // Number of milliseconds between checks to see if the secondaries are connected.
#define MANUAL_INPUT_PINS 15, 9, 14, 10  // Digital pin #s for the manual input buttons.

#define DEBOUNCE_INTERVAL 100 // Interval (msec) to wait before re-reading manual input.

// Some shortcut definitions for the status display
#define DISPLAY_DASH 0b01000000  // Segment G of the 7-segment display.
#define DISPLAY_EMPTY 0
#define DISPLAY_DOT 0b10000000

// Function prototypes
void initializeIO();
void populateSecondaries();
void startBLEService();
void readSettingsFromBLE();
void readSettingsFromManualInputs();
void setManualStyle(uint style);
SecondaryClient* scanForSecondary();
void consolidateTotalsAndWriteToSecondaries();
void setStatusDisplay(byte digit1, byte digit2, byte digit3, byte digit4);
void checkSecondaryConnections();
void resetSecondaryConnections();
void updateAllSecondaries();