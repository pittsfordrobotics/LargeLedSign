#include "Arduino.h"
#include "BLETypedCharacteristics.h"
#include <ArduinoBLE.h>
#include <vector>
#include <algorithm>
#include "BluetoothCommon.h"
#include "CommonPeripheral.h"
#include "SecondaryClient.h"
#include <TM1637Display.h>

#define TM1637_CLOCK 8    // Digital pin # for the TM1637 clock line
#define TM1637_DIO 7      // Digital pin # for the TM1637 data line
#define TM1637_BRIGHTNESS 5  // Brightness of the TM1637, between 0 and 7

// Function prototypes
void initializeIO();
void populateSecondaries();
void startBLEService();
void readBLE();
void readManualInputs();
SecondaryClient* scanForSecondary();
void consolidateTotalsAndWriteToSecondaries();
void setStatusDisplay(byte digit1, byte digit2, byte digit3, byte digit4);