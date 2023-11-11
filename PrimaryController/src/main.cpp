#include "main.h"

// Global variables
BLEService ledService{ BTCOMMON_PRIMARYCONTROLLER_UUID };
BLEByteCharacteristic brightnessCharacteristic{ BTCOMMON_BRIGHTNESSCHARACTERISTIC_UUID, BLERead | BLENotify | BLEWrite };
BLECharacteristic* remoteBrightnessCharacteristic1;
BLECharacteristic* remoteBrightnessCharacteristic2;

std::vector<BLEDevice*> allSecondaries;
ulong nextUpdate = 0;

void setup() {
  delay(500);
  Serial.begin(9600);
  Serial.println("Starting...");
    
  if (!BLE.begin()) {
    Serial.println("BLE initialization failed!");
    while(true) {}
  }
  
  startBLEService();
  populateSecondaries();
}

void populateSecondaries() {
  int numberExpected = 2;
  for (int i = 0; i < numberExpected; i++) {
    allSecondaries.push_back(scanForSecondary());
  }
}

void loop() {
  // poll BLE to enable callbacks
  //BLE.poll();

  bool atLeastOneDisconnected = false;

  if (nextUpdate < millis()) {
    for (uint i = 0; i < allSecondaries.size(); i++) {
      Serial.print("Secondary [");
      Serial.print(allSecondaries.at(i)->localName());
      Serial.print("]");
      if (allSecondaries.at(i)->connected()) {
        Serial.println(" is connected.");
      } else {
        Serial.println(" has been disconnected.");
        atLeastOneDisconnected = true;
      }
    }
    nextUpdate = millis() + 1000;
  }

  if (atLeastOneDisconnected) {
    // Taking the easy way out...
    // Rather than trying to pinpoint which one (or more) of the devices
    // dropped the connection and trying to only re-establish those connections,
    // we'll just disconnect all secondaries and reconnect all of them.
    Serial.println("Resetting all secondary devices and reconnecting...");
    for (uint i = 0; i < allSecondaries.size(); i++) {
      if (allSecondaries.at(i)->connected()) {
        allSecondaries.at(i)->disconnect();
      }
      delete allSecondaries.at(i);
    }
    allSecondaries.clear();
    populateSecondaries();
  }
}

BLEDevice* scanForSecondary() {
  Serial.print("Scanning for peripherals with uuid = ");
  Serial.println(BTCOMMON_SECONDARYCONTROLLER_UUID);
  BLE.scanForUuid(BTCOMMON_SECONDARYCONTROLLER_UUID, true);

  BLEDevice peripheral = BLE.available();
  while (!peripheral) {
    peripheral = BLE.available();
  }

  String localName = "unknown";
  if (peripheral) {
    Serial.println("Found peripheral.");
    if (peripheral.hasLocalName()) {
      localName = peripheral.localName();
      Serial.print("Peripheral name: ");
      Serial.println(localName);
    }
  }

  BLE.stopScan();

  if (peripheral.connect()) {
    Serial.print("Connected to ");
    Serial.println(localName);
  } else {
    Serial.println("Failed to connect!");
  }

  if (peripheral.discoverAttributes()) {
    //Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
  }

  // Verify characteristics are available (likely not needed, just testing for now.)
  BLECharacteristic characteristic = peripheral.characteristic("5eccb54e-465f-47f4-ac50-6735bfc0e730");

  if (!characteristic) {
    Serial.println("Peripheral does not have brightness characteristic!");
    peripheral.disconnect();
  } else if (!characteristic.canWrite()) {
    Serial.println("Peripheral does not have a writable brightness characteristic!");
    peripheral.disconnect();
  }

  return new BLEDevice(peripheral);
}

void startBLEService() {
  Serial.println("Setting up Peripheral service.");
  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  BLE.setLocalName("Primary POC");
  BLE.setAdvertisedService(ledService);
  brightnessCharacteristic.setValue(0);
  brightnessCharacteristic.setEventHandler(BLEWritten, brightnessCharacteristicWritten);
  ledService.addCharacteristic(brightnessCharacteristic);
  BLE.addService(ledService);
  BLE.advertise();
  Serial.println("Peripheral service started.");
}

void debugCheckCentral()
{
  BLEDevice central = BLE.central();
  if (central) {
    Serial.println("Connected to central.");
    int value = brightnessCharacteristic.value();
    Serial.print("Brightness: ");
    Serial.println(value);
  }
}

void brightnessCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
  // central wrote new value to characteristic, update LED
  byte value = brightnessCharacteristic.value();
  Serial.print("Brightness value written: ");
  Serial.println(value);

  remoteBrightnessCharacteristic1->writeValue(value);
  remoteBrightnessCharacteristic2->writeValue(value);
}

void blePeripheralConnectHandler(BLEDevice central) {
  // central connected event handler
  Serial.print("Connected event, central: ");
  Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLEDevice central) {
  // central disconnected event handler
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());
}