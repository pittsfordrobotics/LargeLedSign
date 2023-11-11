#include "Arduino.h"
#include "BLETypedCharacteristics.h"
#include <ArduinoBLE.h>
#include <vector>
#include "BluetoothCommon.h"

// Function prototypes
void brightnessCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic);
void blePeripheralConnectHandler(BLEDevice central); 
void blePeripheralDisconnectHandler(BLEDevice central); 
void populateSecondaries();
void startBLEService();
BLEDevice* scanForSecondary();

