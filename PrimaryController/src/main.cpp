#include "main.h"

// Global variables
CommonPeripheral btService;
TM1637Display statusDisplay(TM1637_CLOCK, TM1637_DIO);

std::vector<SecondaryClient*> allSecondaries;
ulong nextConnectionCheck = 0;

// Some non-standard display segments
const byte DASH = SEG_G;
const byte EMPTY = 0;

void setup() {
  delay(500);
  Serial.begin(9600);
  Serial.println("Starting...");
  
  statusDisplay.setBrightness(TM1637_BRIGHTNESS);
  setStatusDisplay(DASH, DASH, DASH, DASH);
  
  if (!BLE.begin()) {
    Serial.println("BLE initialization failed!");
    // Display E-1
    setStatusDisplay(statusDisplay.encodeDigit(14), DASH, statusDisplay.encodeDigit(1), EMPTY);
    while(true) {}
  }
  
  populateSecondaries();
  consolidateTotalsAndWriteToSecondaries();
  // todo:
  // Calculate all the digit/column/pixel offsets and write them back to the peripherals.
  startBLEService();
}

byte lastBrightness = 0;
byte currentBrightness = 0;
void loop() {
  if (btService.isConnected()) {
    setStatusDisplay(EMPTY, DASH, DASH, EMPTY);
  } else {
    statusDisplay.clear();
  }

  currentBrightness = btService.getBrightness();
  if (currentBrightness != lastBrightness) {
    Serial.println("Brightness was changed.");
    lastBrightness = currentBrightness;
  }
  
}

void initialzeIO() {
  // Initialize the manual IO pins
}

void setStatusDisplay(byte digit1, byte digit2, byte digit3, byte digit4) {
  byte digits[] = {digit1, digit2, digit3, digit4};
  statusDisplay.setSegments(digits);
}

void checkSecondaryConnections() {
  bool atLeastOneDisconnected = false;
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

  if (atLeastOneDisconnected) {
    resetSecondaryConnections();
  }
}

void resetSecondaryConnections() {
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

void populateSecondaries() {
  // Todo:
  // Read the number of expected secondaries based on input.
  byte numberExpected = 2;
  setStatusDisplay(EMPTY, statusDisplay.encodeDigit(0), DASH, statusDisplay.encodeDigit(numberExpected));
  while (allSecondaries.size() < numberExpected) {
    SecondaryClient* secondary = scanForSecondary();
    if (secondary->isValidClient()) {
      allSecondaries.push_back(secondary);
      setStatusDisplay(EMPTY, statusDisplay.encodeDigit(allSecondaries.size()), DASH, statusDisplay.encodeDigit(numberExpected));
    }
  }

  // We found all we were looking for. Order them by position.
  std::sort(
    allSecondaries.begin(),
    allSecondaries.end(),
    [](SecondaryClient* &a, SecondaryClient* &b){ return a->getSignOrder() < b->getSignOrder(); });

  statusDisplay.clear();
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
  // Status: B1
  setStatusDisplay(EMPTY, EMPTY, statusDisplay.encodeDigit(11), statusDisplay.encodeDigit(1));
  Serial.println("Setting up Peripheral service using common logic.");
  String localName = "3181 LED Controller Primary";
  btService.initialize(BTCOMMON_PRIMARYCONTROLLER_UUID, localName);

  btService.setPatternNames("p1;p2");
  btService.setStyleNames("s1;s2");
  btService.setBrightness(1);
  btService.setPattern(0);
  btService.setSpeed(10);
  btService.setStep(15);
  btService.setStyle(1);

  return;

  // Set the various characteristics based on what's read from any one of the secondaries.
  // Status: B2
  setStatusDisplay(EMPTY, EMPTY, statusDisplay.encodeDigit(11), statusDisplay.encodeDigit(2));
  Serial.println("Proxying characteristics.");
  
  btService.setPatternNames(allSecondaries.at(0)->getStringValue(BTCOMMON_PATTERNNAMESCHARACTERISTIC_UUID));
  btService.setStyleNames(allSecondaries.at(0)->getStringValue(BTCOMMON_STYLENAMESCHARACTERISTIC_UUID));
  btService.setBrightness(allSecondaries.at(0)->getByteValue(BTCOMMON_BRIGHTNESSCHARACTERISTIC_UUID));
  btService.setPattern(allSecondaries.at(0)->getByteValue(BTCOMMON_PATTERNCHARACTERISTIC_UUID));
  btService.setSpeed(allSecondaries.at(0)->getByteValue(BTCOMMON_SPEEDCHARACTERISTIC_UUID));
  btService.setStep(allSecondaries.at(0)->getByteValue(BTCOMMON_STEPCHARACTERISTIC_UUID));
  btService.setStyle(allSecondaries.at(0)->getByteValue(BTCOMMON_STYLECHARACTERISTIC_UUID));

  Serial.println("Peripheral service started.");
  statusDisplay.clear();
}

void readBLE() {
  // Actually do something here to read characteristics and update the secondaries.
}

void readManualInputs() {
  // Check the digital inputs to see if we need to change states.
  // Phase 1 - single-state input. ie, pushing button A changes to state A. Pushing A a second time has no effect.
  // Phase 2 (if needed) - dual-state. ie, pushing button A changes to state A1. Pushing A again changes to state A2, then back to A1.
}