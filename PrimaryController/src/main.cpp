#include "main.h"

// Global variables
BLEService ledService{ BTCOMMON_PRIMARYCONTROLLER_UUID };
BLEByteCharacteristic brightnessCharacteristic{ BTCOMMON_BRIGHTNESSCHARACTERISTIC_UUID, BLERead | BLENotify | BLEWrite };
BLECharacteristic* remoteBrightnessCharacteristic1;
BLECharacteristic* remoteBrightnessCharacteristic2;
CommonPeripheral btService;

std::vector<SecondaryClient*> allSecondaries;
ulong nextUpdate = 0;

void setup() {
  delay(500);
  Serial.begin(9600);
  Serial.println("Starting...");
    
  if (!BLE.begin()) {
    Serial.println("BLE initialization failed!");
    // todo: set the status display
    while(true) {}
  }
  
  populateSecondaries();
  consolidateTotalsAndWriteToSecondaries();
  // todo:
  // Calculate all the digit/column/pixel offsets and write them back to the peripherals.
  startBLEService();
}

void loop() {
  bool atLeastOneDisconnected = false;

  if (nextUpdate < millis()) {
    for (uint i = 0; i < allSecondaries.size(); i++) {
      Serial.print("Secondary [");
      Serial.print(allSecondaries.at(i)->getLocalName());
      Serial.print("]");
      if (allSecondaries.at(i)->isConnected()) {
        Serial.println(" is connected.");
      } else {
        Serial.println(" has been disconnected.");
        atLeastOneDisconnected = true;
      }
    }
    nextUpdate = millis() + 2000;
  }

  if (atLeastOneDisconnected) {
    // Taking the easy way out...
    // Rather than trying to pinpoint which one (or more) of the devices
    // dropped the connection and trying to only re-establish those connections,
    // we'll just disconnect all secondaries and reconnect all of them.
    Serial.println("Resetting all secondary devices and reconnecting...");
    for (uint i = 0; i < allSecondaries.size(); i++) {
      if (allSecondaries.at(i)->isConnected()) {
        // deleting the secondary will disconnect the peripheral automatically, but do it here anyways.
        allSecondaries.at(i)->disconnect();
      }
      delete allSecondaries.at(i);
    }
    allSecondaries.clear();
    populateSecondaries();
    // Not bothering to recalculate totals at this point...
    // The assumption is that the connection was interrupted due to signal loss to the client,
    // not power loss at the client.  The client should still retain all the sign data needed
    // to properly operate.
    // If it turns out that client power loss or client reset is an issue, we should recalculate
    // the totals and republish them to all clients.
  }
}

void initialzeIO() {
  // Initialize the manual IO pins
  // Initialize the status screen pins
}

void populateSecondaries() {
  // Todo:
  // Read the number of expected secondaries based on input.
  uint numberExpected = 2;
  while (allSecondaries.size() < numberExpected) {
    SecondaryClient* secondary = scanForSecondary();
    if (secondary->isValidClient()) {
      allSecondaries.push_back(secondary);
      // Todo: update the status display
    }
  }

  // We found all we were looking for. Order them by position.
  std::sort(
    allSecondaries.begin(),
    allSecondaries.end(),
    [](SecondaryClient* &a, SecondaryClient* &b){ return a->getSignOrder() < b->getSignOrder(); });
}

SecondaryClient* scanForSecondary() {
  Serial.print("Scanning for peripherals with uuid = ");
  Serial.println(BTCOMMON_SECONDARYCONTROLLER_UUID);
  bool allowDuplicateAdvertisements = true;
  BLE.scanForUuid(BTCOMMON_SECONDARYCONTROLLER_UUID, allowDuplicateAdvertisements);

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

  return new SecondaryClient(peripheral);
}

void consolidateTotalsAndWriteToSecondaries() {
  // Sum up the number of digits, columns, pixels.
  // Format the extended sign data and emit it back to the secondaries.
  // Extended sign data format:
  // type;order;numberOfColumns;numberOfPixels;digitsL;columnsL;pixelsL;digitsR;columnsR;pixelsR
}

void startBLEService() {
  Serial.println("Setting up Peripheral service using common logic.");
  
  btService.initialize(BTCOMMON_PRIMARYCONTROLLER_UUID, "Primary POC");

  Serial.println("Peripheral service started.");
}

void readBLE() {
  // Actually do something here to read characteristics and update the secondaries.
}

void readManualInputs() {
  // Check the digital inputs to see if we need to change states.
  // Phase 1 - single-state input. ie, pushing button A changes to state A. Pushing A a second time has no effect.
  // Phase 2 (if needed) - dual-state. ie, pushing button A changes to state A1. Pushing A again changes to state A2, then back to A1.
}