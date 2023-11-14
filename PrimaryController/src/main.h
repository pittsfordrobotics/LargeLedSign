#include "Arduino.h"
#include "BLETypedCharacteristics.h"
#include <ArduinoBLE.h>
#include <vector>
#include <algorithm>
#include "BluetoothCommon.h"
#include "CommonPeripheral.h"
#include "SecondaryClient.h"

// Function prototypes
void initializeIO();
void populateSecondaries();
void startBLEService();
void readBLE();
void readManualInputs();
SecondaryClient* scanForSecondary();
void consolidateTotalsAndWriteToSecondaries();