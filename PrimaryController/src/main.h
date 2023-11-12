#include "Arduino.h"
#include "BLETypedCharacteristics.h"
#include <ArduinoBLE.h>
#include <vector>
#include "BluetoothCommon.h"
#include "CommonPeripheral.h"

// Function prototypes
void populateSecondaries();
void startBLEService();
BLEDevice* scanForSecondary();

