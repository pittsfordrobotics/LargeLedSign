#include "Arduino.h"
#include "BLETypedCharacteristics.h"
#include <ArduinoBLE.h>
#include <vector>
#include "BluetoothCommon.h"
#include "CommonPeripheral.h"
#include "SecondaryClient.h"

// Function prototypes
void populateSecondaries();
void startBLEService();
SecondaryClient* scanForSecondary();

