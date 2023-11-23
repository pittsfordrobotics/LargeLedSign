#include "main.h"

// Global variables
CommonPeripheral btService;
TM1637Display statusDisplay(TM1637_CLOCK, TM1637_DIO);

std::vector<ManualButton*> manualInputButtons;
std::vector<SecondaryClient*> allSecondaries;
ulong nextConnectionCheck = 0;
ulong lastButtonPress = 0;
bool resetRequested = false;
bool shouldIgnoreLogo = false;
ulong loopCounter = 0;
ulong lastTelemetryTimestamp = 0;
SignStatus lastServiceStatus;
SignStatus currentServiceStatus;

void setup() {
  delay(500);
  Serial.begin(9600);
  Serial.println("Starting...");
  
  initializeIO();
  
  // If manual button 1 is pressed (ie, LOW), don't look for the logo.
  shouldIgnoreLogo = digitalRead(manualInputButtons[0]->rawPinStatus()) == HIGH;

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
  startBLEService();
  setManualStyle(0);
  resetRequested = true;
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

  checkSecondaryConnections();
  readSettingsFromBLE();
  // Manual inputs will override BLE settings, so read them last.
  updateInputButtons();
  readSettingsFromManualInputs();

  if (currentServiceStatus != lastServiceStatus || resetRequested) {
    resetRequested = false;
    updateAllSecondaries();
    lastServiceStatus = currentServiceStatus;
  }

  // TEST FOR LONG PRESS
  for (uint i = 0; i < manualInputButtons.size(); i++) {
    if (manualInputButtons[i]->wasPressed() && manualInputButtons[i]->lastPressType() == ButtonPressType::Long) {
      // ...
      Serial.print("Manual button ");
      Serial.print(i);
      Serial.println(" was long-pressed.");
      manualInputButtons[i]->clearPress();
    }
  }

  updateTelemetry();
}

void initializeIO() {
  // Initialize the manual IO pins
  std::vector<int> manualInputPins { MANUAL_INPUT_PINS };
  for (uint i = 0; i < manualInputPins.size(); i++) {
    manualInputButtons.push_back(new ManualButton(manualInputPins[i], INPUT_PULLUP));
  }
}

void updateInputButtons() {
  for (uint i = 0; i < manualInputButtons.size(); i++) {
    manualInputButtons[i]->update();
  }
}

void setStatusDisplay(byte digit1, byte digit2, byte digit3, byte digit4) {
  byte digits[] = {digit1, digit2, digit3, digit4};
  statusDisplay.setSegments(digits);
}

void checkSecondaryConnections() {
  if (millis() < nextConnectionCheck || btService.isConnected()) {
    return;
  }

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

  nextConnectionCheck = millis() + CONNECTION_CHECK_INTERVAL;
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
    consolidateTotalsAndWriteToSecondaries();
    setManualStyle(0);
    resetRequested = true;
}

void populateSecondaries() {
  byte numberExpected = 2;

  if (shouldIgnoreLogo) {
    numberExpected -= 1;
  }

  setStatusDisplay(statusDisplay.encodeDigit(12), statusDisplay.encodeDigit(0), DISPLAY_DASH, statusDisplay.encodeDigit(numberExpected));
  while (allSecondaries.size() < numberExpected) {
    bool secondaryAdded = false;
    SecondaryClient* secondary = scanForSecondary();
    if (secondary->isValidClient()) {
      // Could use 'secondary->getServiceStatus().getSignConfigurationData().getSignType()',
      // but it seems easier to rely on the fact that the local name ends with <position>-<type>.
      if (!shouldIgnoreLogo || !secondary->getLocalName().endsWith("-15")) {
        allSecondaries.push_back(secondary);
        setStatusDisplay(statusDisplay.encodeDigit(12), statusDisplay.encodeDigit(allSecondaries.size()), DISPLAY_DASH, statusDisplay.encodeDigit(numberExpected));
        secondaryAdded = true;
      }
    }
    if (!secondaryAdded) {
      // Clean up the memory from the invalid peripheral
      delete secondary;
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
    SignStatus status = allSecondaries[i]->getSignStatus();
    signConfigurations.push_back(status.getSignConfigurationData());
  }

  int numCols = 0;
  for (uint i = 0; i < numDigits; i++) {
    numCols += signConfigurations[i].getColumnCount();
  }

  int colsSoFar = 0;
  for (uint i = 0; i < numDigits; i++) {
    setStatusDisplay(statusDisplay.encodeDigit(12), DISPLAY_DASH, DISPLAY_DASH, statusDisplay.encodeDigit(i+1));
    SignOffsetData offsetData;
    offsetData.setDigitsToLeft(i);
    offsetData.setDigitsToRight(numDigits - i - 1);
    offsetData.setColumnsToLeft(colsSoFar);
    offsetData.setColumnsToRight(numCols - colsSoFar - signConfigurations[i].getColumnCount());
    colsSoFar += signConfigurations[i].getColumnCount();
    
    // Write the data back to the secondary
    allSecondaries[i]->setSignOffsetData(offsetData.getOffsetDataString());
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
  
  SignStatus status = allSecondaries.at(0)->getSignStatus();
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
  for (uint i = 0; i < manualInputButtons.size(); i++) {
    // TODO: use .wasPressed.
    // Issue: .wasPressed resets the button status, so a long press won't be recognized.
    // Maybe add a manual .clear method.
    if (manualInputButtons[i]->wasPressed() && manualInputButtons[i]->lastPressType() == ButtonPressType::Normal) {
      manualInputButtons[i]->clearPress();
      Serial.print("Manual style selected: ");
      Serial.println(i);
      setManualStyle(i);
      lastButtonPress = millis();
      resetRequested = true;
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
  
  // Update the local BLE settings to reflect the new manual settings.
  btService.setBrightness(currentServiceStatus.getBrightness());
  btService.setPattern(currentServiceStatus.getPattern());
  btService.setSpeed(currentServiceStatus.getSpeed());
  btService.setStep(currentServiceStatus.getStep());
  btService.setStyle(currentServiceStatus.getStyle());
}

void updateTelemetry()
{
  loopCounter++;
  unsigned long timestamp = millis();

  if (timestamp > lastTelemetryTimestamp + TELEMETRYINTERVAL)
  {
    // Calculate loop timing data
    unsigned long diff = timestamp - lastTelemetryTimestamp;
    double timePerIteration = (double)diff / loopCounter;
    Serial.print(loopCounter);
    Serial.print(" iterations done in ");
    Serial.print(diff);
    Serial.print(" msec; avg msec per iteration: ");
    Serial.println(timePerIteration);
    lastTelemetryTimestamp = timestamp;
    loopCounter = 0;

    // Get battery voltage levels for secondaries
    for (uint i = 0; i < allSecondaries.size(); i++) {
      Serial.print("Battery voltage for sign ");
      Serial.print(allSecondaries[i]->getSignOrder());
      Serial.print(": ");
      Serial.println(allSecondaries[i]->getBatteryVoltage());
    }
  }
}
