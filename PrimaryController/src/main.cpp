#include "main.h"

// Global variables
CommonPeripheral btService;
TM1637Display statusDisplay(TM1637_CLOCK, TM1637_DIO);

std::vector<int> manualInputPins { MANUAL_INPUT_PINS };
std::vector<SecondaryClient*> allSecondaries;
ulong nextConnectionCheck = 0;

ServiceStatus lastServiceStatus;
ServiceStatus currentServiceStatus;

void setup() {
  delay(500);
  Serial.begin(9600);
  Serial.println("Starting...");
  
  statusDisplay.setBrightness(TM1637_BRIGHTNESS);
  setStatusDisplay(DISPLAY_DASH, DISPLAY_DASH, DISPLAY_DASH, DISPLAY_DASH);
  
  if (!BLE.begin()) {
    Serial.println("BLE initialization failed!");
    // Display E-1
    setStatusDisplay(statusDisplay.encodeDigit(14), DISPLAY_DASH, statusDisplay.encodeDigit(1), DISPLAY_EMPTY);
    while(true) {}
  }
  
  populateSecondaries();
  consolidateTotalsAndWriteToSecondaries();
  // todo:
  // Calculate all the digit/column/pixel offsets and write them back to the peripherals.
  startBLEService();
}

void loop() {
  BLE.poll();
  if (btService.isConnected()) {
    // Set the display to "--" to show something connected to us.
    setStatusDisplay(DISPLAY_EMPTY, DISPLAY_DASH, DISPLAY_DASH, DISPLAY_EMPTY);
  } else {
    statusDisplay.clear();
  }

  currentServiceStatus = readBLEInputs();
  if (currentServiceStatus != lastServiceStatus) {
    Serial.println("Status change detected.");
    updateAllSecondaries();
    lastServiceStatus = currentServiceStatus;
  }

}

void initialzeIO() {
  // Initialize the manual IO pins
  for (uint i = 0; i < manualInputPins.size(); i++) {
    pinMode(manualInputPins[i], INPUT_PULLUP);
  }
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
  setStatusDisplay(DISPLAY_EMPTY, statusDisplay.encodeDigit(0), DISPLAY_DASH, statusDisplay.encodeDigit(numberExpected));
  while (allSecondaries.size() < numberExpected) {
    SecondaryClient* secondary = scanForSecondary();
    if (secondary->isValidClient()) {
      allSecondaries.push_back(secondary);
      setStatusDisplay(DISPLAY_EMPTY, statusDisplay.encodeDigit(allSecondaries.size()), DISPLAY_DASH, statusDisplay.encodeDigit(numberExpected));
    }
  }

  // We found all we were looking for. Order them by position, lowest position first.
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
  setStatusDisplay(DISPLAY_EMPTY, DISPLAY_EMPTY, statusDisplay.encodeDigit(11), statusDisplay.encodeDigit(1));
  Serial.println("Setting up Peripheral service using common logic.");
  String localName = "3181 LED Controller Primary";
  btService.initialize(BTCOMMON_PRIMARYCONTROLLER_UUID, localName);

  // Set the various characteristics based on what's read from any one of the secondaries.
  // Status: B2
  setStatusDisplay(DISPLAY_EMPTY, DISPLAY_EMPTY, statusDisplay.encodeDigit(11), statusDisplay.encodeDigit(2));
  Serial.println("Proxying characteristics.");
  
  ServiceStatus status = allSecondaries.at(0)->getServiceStatus();
  btService.setPatternNames(status.getPatternNames());
  btService.setStyleNames(status.getStyleNames());
  btService.setBrightness(status.getBrightness());
  btService.setPattern(status.getPattern());
  btService.setSpeed(status.getSpeed());
  btService.setStep(status.getStep());
  btService.setStyle(status.getStyle());

  Serial.println("Peripheral service started.");
  statusDisplay.clear();
}

ServiceStatus readBLEInputs() {
  ServiceStatus currentStatus;
  currentStatus.setBrightness(btService.getBrightness());
  currentStatus.setPattern(btService.getPattern());
  currentStatus.setSpeed(btService.getSpeed());
  currentStatus.setStep(btService.getStep());
  currentStatus.setStyle(btService.getStyle());

  return currentStatus;
}

ServiceStatus readManualInputs() {
  // Check the digital inputs to see if we need to change states.
  // Phase 1 - single-state input. ie, pushing button A changes to state A. Pushing A a second time has no effect.
  // Phase 2 (if needed) - dual-state. ie, pushing button A changes to state A1. Pushing A again changes to state A2, then back to A1.
  
  ServiceStatus s;
  return s;
}

void updateAllSecondaries() {
  for (uint i = 0; i < allSecondaries.size(); i++) {
    allSecondaries[i]->setBrightness(currentServiceStatus.getBrightness());
  }
  for (uint i = 0; i < allSecondaries.size(); i++) {
    allSecondaries[i]->setSpeed(currentServiceStatus.getSpeed());
  }
  for (uint i = 0; i < allSecondaries.size(); i++) {
    allSecondaries[i]->setStep(currentServiceStatus.getStep());
  }
  for (uint i = 0; i < allSecondaries.size(); i++) {
    allSecondaries[i]->setStyle(currentServiceStatus.getStyle());
  }
  for (uint i = 0; i < allSecondaries.size(); i++) {
    allSecondaries[i]->setPattern(currentServiceStatus.getPattern());
  }

}