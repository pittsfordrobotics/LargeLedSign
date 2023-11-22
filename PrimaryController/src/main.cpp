#include "main.h"

// Global variables
CommonPeripheral btService;
TM1637Display statusDisplay(TM1637_CLOCK, TM1637_DIO);

std::vector<int> manualInputPins { MANUAL_INPUT_PINS };
std::vector<SecondaryClient*> allSecondaries;
ulong nextConnectionCheck = 0;
ulong lastButtonPress = 0;

ServiceStatus lastServiceStatus;
ServiceStatus currentServiceStatus;

void setup() {
  delay(500);
  Serial.begin(9600);
  Serial.println("Starting...");
  
  initializeIO();
  statusDisplay.setBrightness(TM1637_BRIGHTNESS);
  setStatusDisplay(DISPLAY_DASH, DISPLAY_DASH, DISPLAY_DASH, DISPLAY_DASH);
  
  if (!BLE.begin()) {
    Serial.println("BLE initialization failed!");
    // Display E-1
    setStatusDisplay(statusDisplay.encodeDigit(14), DISPLAY_DASH, statusDisplay.encodeDigit(1), DISPLAY_EMPTY);
    while(true) {}
  }
  
  populateSecondaries();
  // todo:
  // Calculate all the digit/column/pixel offsets and write them back to the peripherals.
  consolidateTotalsAndWriteToSecondaries();
  setManualStyle(0);
  startBLEService();
  nextConnectionCheck = millis() + CONNECTION_CHECK_INTERVAL;
}

void loop() {
  BLE.poll();
  if (btService.isConnected()) {
    // Set the display to "--" to show something connected to us.
    setStatusDisplay(DISPLAY_EMPTY, DISPLAY_DASH, DISPLAY_DASH, DISPLAY_EMPTY);
  } else {
    statusDisplay.clear();
  }

  if (millis() > nextConnectionCheck) {
    if (!btService.isConnected()) {
      // Verify we're still connected to all secondaries.
      checkSecondaryConnections();
      nextConnectionCheck = millis() + CONNECTION_CHECK_INTERVAL;
    }
  }

  readSettingsFromBLE();
  // Manual inputs will override BLE settings, so read them last.
  readSettingsFromManualInputs();

  if (currentServiceStatus != lastServiceStatus) {
    Serial.println("Status change detected.");
    updateAllSecondaries();
    lastServiceStatus = currentServiceStatus;
  }
}

void initializeIO() {
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
  setStatusDisplay(DISPLAY_DOT, DISPLAY_EMPTY, DISPLAY_EMPTY, DISPLAY_EMPTY);
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
  statusDisplay.clear();

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
  setStatusDisplay(statusDisplay.encodeDigit(12), statusDisplay.encodeDigit(0), DISPLAY_DASH, statusDisplay.encodeDigit(numberExpected));
  while (allSecondaries.size() < numberExpected) {
    SecondaryClient* secondary = scanForSecondary();
    if (secondary->isValidClient()) {
      allSecondaries.push_back(secondary);
      setStatusDisplay(statusDisplay.encodeDigit(12), statusDisplay.encodeDigit(allSecondaries.size()), DISPLAY_DASH, statusDisplay.encodeDigit(numberExpected));
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
  // Status C---
  setStatusDisplay(statusDisplay.encodeDigit(12), DISPLAY_DASH, DISPLAY_DASH, DISPLAY_DASH);
  uint numDigits = allSecondaries.size();

  // Stash the sign config data for each secondary so we don't retrieve it every time.
  std::vector<SignConfigurationData> signConfigurations;
  for (uint i = 0; i < numDigits; i++) {
    ServiceStatus status = allSecondaries[i]->getServiceStatus();
    signConfigurations.push_back(status.getSignConfigurationData());
  }

  int numCols = 0;
  int numPixels = 0;
  for (uint i = 0; i < numDigits; i++) {
    numCols += signConfigurations[i].getColumnCount();
    numPixels += signConfigurations[i].getPixelCount();
  }

  int colsSoFar = 0;
  int pixelsSoFar = 0;
  for (uint i = 0; i < numDigits; i++) {
    setStatusDisplay(statusDisplay.encodeDigit(12), DISPLAY_DASH, DISPLAY_DASH, statusDisplay.encodeDigit(i+1));
    SignConfigurationData signConfigData(signConfigurations[i]);
    signConfigData.setDigitsToLeft(i);
    signConfigData.setDigitsToRight(numDigits - i - 1);
    signConfigData.setColumnsToLeft(colsSoFar);
    signConfigData.setColumnsToRight(numCols - colsSoFar - signConfigData.getColumnCount());
    colsSoFar += signConfigurations[i].getColumnCount();
    signConfigData.setPixelsToLeft(pixelsSoFar);
    signConfigData.setPixelsToRight(numPixels - pixelsSoFar - signConfigData.getPixelCount());
    pixelsSoFar += signConfigurations[i].getPixelCount();
    
    // Write the data back to the secondary
    allSecondaries[i]->setSignConfigurationData(signConfigData.getConfigurationString());
  }

  statusDisplay.clear();
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

void readSettingsFromBLE() {
  currentServiceStatus.setBrightness(btService.getBrightness());
  currentServiceStatus.setPattern(btService.getPattern());
  currentServiceStatus.setSpeed(btService.getSpeed());
  currentServiceStatus.setStep(btService.getStep());
  currentServiceStatus.setStyle(btService.getStyle());
}

void readSettingsFromManualInputs() {
  if ((lastButtonPress + DEBOUNCE_INTERVAL) > millis()) {
    // In the de-bounce interval.
    return;
  }

  // Check the digital inputs to see if we need to change states.
  // A button press is indicated by the input getting pulled low.
  // At this point, we're not doing anything complicated such as
  // looking for button combinations.
  // Just look for the first button that's been pressed.
  for (uint i = 0; i < manualInputPins.size(); i++) {
    if (digitalRead(manualInputPins[i]) == LOW) {
      Serial.print("Manual style selected: ");
      Serial.println(i);
      setManualStyle(i);
      lastButtonPress = millis();
      return;
    }
  }
}

void updateAllSecondaries() {
  for (uint i = 0; i < allSecondaries.size(); i++) {
    allSecondaries[i]->setBrightness(currentServiceStatus.getBrightness());
    allSecondaries[i]->setSpeed(currentServiceStatus.getSpeed());
    allSecondaries[i]->setStep(currentServiceStatus.getStep());
    allSecondaries[i]->setStyle(currentServiceStatus.getStyle());
    allSecondaries[i]->setPattern(currentServiceStatus.getPattern());
  }
  ulong timestamp = millis();
  for (uint i = 0; i < allSecondaries.size(); i++) {
    allSecondaries[i]->updateSyncData(timestamp);
  }
}

void setManualStyle(uint style) {
  // These style settings will need to change if the ordering of
  // styles/patterns changes.
  switch (style) {
    case 0:
      // Solid Pink
      currentServiceStatus.setBrightness(10);
      currentServiceStatus.setPattern(0);
      currentServiceStatus.setSpeed(1);
      currentServiceStatus.setStep(1);
      currentServiceStatus.setStyle(1);
      break;
    case 1:
      // Red-Pink
      currentServiceStatus.setBrightness(10);
      currentServiceStatus.setPattern(1);
      currentServiceStatus.setSpeed(25);
      currentServiceStatus.setStep(25);
      currentServiceStatus.setStyle(4);
      break;
    case 2:
      // Blue-Pink
      currentServiceStatus.setBrightness(10);
      currentServiceStatus.setPattern(1);
      currentServiceStatus.setSpeed(25);
      currentServiceStatus.setStep(25);
      currentServiceStatus.setStyle(2);
      break;
    default:
      // Rainbow
      currentServiceStatus.setBrightness(10);
      currentServiceStatus.setPattern(1);
      currentServiceStatus.setSpeed(85);
      currentServiceStatus.setStep(95);
      currentServiceStatus.setStyle(0);
  }
  // TODO: if a button was pressed (even if it was the same style),
  // update the "sync" signal to reset the secondaries.
  
  // Update the BLE settings to reflect the new manual settings.
  btService.setBrightness(currentServiceStatus.getBrightness());
  btService.setPattern(currentServiceStatus.getPattern());
  btService.setSpeed(currentServiceStatus.getSpeed());
  btService.setStep(currentServiceStatus.getStep());
  btService.setStyle(currentServiceStatus.getStyle());
}