#include "Arduino.h"
#include "BLETypedCharacteristics.h"
#include <ArduinoBLE.h>
#include <vector>
#include "BluetoothCommon.h"

// Function prototypes
void populateSecondaries();
void startBLEService();
void startBLEService_Common();
BLEDevice* scanForSecondary();

