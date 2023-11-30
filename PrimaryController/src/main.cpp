#include "main.h"

// Global variables
CommonPeripheral btService;
StatusDisplay display(TM1637_CLOCK, TM1637_DIO, TM1637_BRIGHTNESS);

std::vector<ManualButton *> manualInputButtons;
std::vector<SecondaryClient *> allSecondaries;
ulong nextConnectionCheck = 0;
bool resetRequested = false;
bool shouldIgnoreLogo = false;
ulong loopCounter = 0;
ulong lastTelemetryTimestamp = 0;
SignStatus lastServiceStatus;
SignStatus currentServiceStatus;

void setup()
{
    Serial.begin(9600);
    delay(500);
    Serial.println("Starting...");

    initializeIO();

    // If manual button 1 is pressed (ie, LOW), don't look for the logo.
    shouldIgnoreLogo = (manualInputButtons[0]->rawPinStatus() == LOW);

    display.setDisplay("----");

    if (!BLE.begin())
    {
        Serial.println("BLE initialization failed!");
        display.setDisplay("E1");
        while (true)
        {
        }
    }

    populateSecondaries();
    consolidateTotalsAndWriteToSecondaries();
    startBLEService();
    setManualStyle(0);
    resetRequested = true;
    nextConnectionCheck = millis() + CONNECTION_CHECK_INTERVAL;
}

void loop()
{
    BLE.poll();
    display.update();

    if (btService.isConnected())
    {
        // Set the display to "--" to show something connected to us.
        // Display it as "temporary" since it's a low-priority message.
        display.displayTemporary(" --", 500);
    }

    checkSecondaryConnections();
    readSettingsFromBLE();
    // Manual inputs will override BLE settings, so read them last.
    processManualInputs();

    if (currentServiceStatus != lastServiceStatus || resetRequested)
    {
        resetRequested = false;
        updateAllSecondaries();
        lastServiceStatus = currentServiceStatus;
    }

    updateTelemetry();
}

void processManualInputs()
{
    updateInputButtons();

    // Look for any button presses that indicate style changes.
    for (uint i = 0; i < manualInputButtons.size(); i++)
    {
        if (manualInputButtons[i]->wasPressed() 
            && (manualInputButtons[i]->lastPressType() == ButtonPressType::Normal || manualInputButtons[i]->lastPressType() == ButtonPressType::Double))
        {
            int style = i;
            if (manualInputButtons[i]->lastPressType() == ButtonPressType::Double)
            {
                style += 4;
            }

            manualInputButtons[i]->clearPress();
            Serial.print("Manual style selected: ");
            Serial.println(style);
            setManualStyle(style);
            resetRequested = true;
        }
    }

    // If button 0 was long-pressed, display battery voltages for the clients.
    if (manualInputButtons[0]->wasPressed() && manualInputButtons[0]->lastPressType() == ButtonPressType::Long)
    {
        manualInputButtons[0]->clearPress();
        displayBatteryVoltages();
    }
}

void displayBatteryVoltages()
{
    std::vector<String> stringsToDisplay;
    for (uint i = 0; i < allSecondaries.size(); i++)
    {
        String voltageString(i + 1);
        voltageString.concat("=");
        voltageString.concat(String(allSecondaries[i]->getBatteryVoltage(), 2));
        stringsToDisplay.push_back(voltageString);
    }

    display.displaySequence(stringsToDisplay, 1500);
}

void initializeIO()
{
    // Initialize the manual IO pins
    std::vector<int> manualInputPins{MANUAL_INPUT_PINS};
    for (uint i = 0; i < manualInputPins.size(); i++)
    {
        manualInputButtons.push_back(new ManualButton(manualInputPins[i], INPUT_PULLUP));
    }
}

void updateInputButtons()
{
    for (uint i = 0; i < manualInputButtons.size(); i++)
    {
        manualInputButtons[i]->update();
    }
}

void checkSecondaryConnections()
{
    if (millis() < nextConnectionCheck || btService.isConnected())
    {
        return;
    }

    display.displayTemporary(" .", 200);
    bool atLeastOneDisconnected = false;
    for (uint i = 0; i < allSecondaries.size(); i++)
    {
        Serial.print("Secondary [");
        Serial.print(allSecondaries.at(i)->getLocalName());
        Serial.print("]");
        if (allSecondaries.at(i)->isConnected())
        {
            Serial.println(" is connected.");
        }
        else
        {
            Serial.println(" has been disconnected.");
            atLeastOneDisconnected = true;
        }
    }

    if (atLeastOneDisconnected)
    {
        resetSecondaryConnections();
    }

    nextConnectionCheck = millis() + CONNECTION_CHECK_INTERVAL;
}

void resetSecondaryConnections()
{
    // Taking the easy way out...
    // Rather than trying to pinpoint which one (or more) of the devices
    // dropped the connection and trying to only re-establish those connections,
    // we'll just disconnect all secondaries and reconnect all of them.
    Serial.println("Resetting all secondary devices and reconnecting...");
    for (uint i = 0; i < allSecondaries.size(); i++)
    {
        if (allSecondaries.at(i)->isConnected())
        {
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

void populateSecondaries()
{
    byte numberExpected = 2;

    if (shouldIgnoreLogo)
    {
        numberExpected -= 1;
    }

    display.setDisplay("C0-" + String(numberExpected));
    while (allSecondaries.size() < numberExpected)
    {
        bool secondaryAdded = false;
        SecondaryClient *secondary = scanForSecondary();
        if (secondary->isValidClient())
        {
            // Could use 'secondary->getServiceStatus().getSignConfigurationData().getSignType()',
            // but it seems easier to rely on the fact that the local name ends with <position>-<type>.
            if (!shouldIgnoreLogo || !secondary->getLocalName().endsWith("-15"))
            {
                allSecondaries.push_back(secondary);
                display.setDisplay("C" + String(allSecondaries.size()) + "-" + String(numberExpected));
                secondaryAdded = true;
            }
        }
        if (!secondaryAdded)
        {
            // Clean up the memory from the invalid peripheral
            delete secondary;
        }
    }

    // We found all we were looking for. Order them by position, lowest position first.
    std::sort(
            allSecondaries.begin(),
            allSecondaries.end(),
            [](SecondaryClient *&a, SecondaryClient *&b)
            { return a->getSignOrder() < b->getSignOrder(); });

    display.clear();
}

SecondaryClient *scanForSecondary()
{
    Serial.print("Scanning for peripherals with uuid = ");
    Serial.println(BTCOMMON_SECONDARYCONTROLLER_UUID);
    bool allowDuplicateAdvertisements = true;
    BLE.scanForUuid(BTCOMMON_SECONDARYCONTROLLER_UUID, allowDuplicateAdvertisements);

    BLEDevice peripheral = BLE.available();
    while (!peripheral)
    {
        peripheral = BLE.available();
    }

    String localName = "unknown";
    if (peripheral)
    {
        Serial.println("Found peripheral.");
        if (peripheral.hasLocalName())
        {
            localName = peripheral.localName();
            Serial.print("Peripheral name: ");
            Serial.println(localName);
        }
    }

    BLE.stopScan();

    if (peripheral.connect())
    {
        Serial.print("Connected to ");
        Serial.println(localName);
    }
    else
    {
        Serial.println("Failed to connect!");
    }

    return new SecondaryClient(peripheral);
}

void consolidateTotalsAndWriteToSecondaries()
{
    display.setDisplay("C---");
    uint numDigits = allSecondaries.size();

    // Stash the sign config data for each secondary so we don't retrieve it every time.
    std::vector<SignConfigurationData> signConfigurations;
    for (uint i = 0; i < numDigits; i++)
    {
        SignStatus status = allSecondaries[i]->getSignStatus();
        signConfigurations.push_back(status.getSignConfigurationData());
    }

    int numCols = 0;
    for (uint i = 0; i < numDigits; i++)
    {
        numCols += signConfigurations[i].getColumnCount();
    }

    int colsSoFar = 0;
    for (uint i = 0; i < numDigits; i++)
    {
        display.setDisplay("C--" + String(i + 1));
        SignOffsetData offsetData;
        offsetData.setDigitsToLeft(i);
        offsetData.setDigitsToRight(numDigits - i - 1);
        offsetData.setColumnsToLeft(colsSoFar);
        offsetData.setColumnsToRight(numCols - colsSoFar - signConfigurations[i].getColumnCount());
        colsSoFar += signConfigurations[i].getColumnCount();

        // Write the data back to the secondary
        allSecondaries[i]->setSignOffsetData(offsetData.getOffsetDataString());
    }

    display.clear();
}

void startBLEService()
{
    display.setDisplay("C  =");
    Serial.println("Setting up Peripheral service using common logic.");
    String localName = "3181 LED Controller Primary";
    btService.initialize(BTCOMMON_PRIMARYCONTROLLER_UUID, localName);

    // Set the various characteristics based on what's read from any one of the secondaries.
    display.setDisplay("C ==");

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
    display.clear();
}

void readSettingsFromBLE()
{
    currentServiceStatus.setBrightness(btService.getBrightness());
    currentServiceStatus.setPattern(btService.getPattern());
    currentServiceStatus.setSpeed(btService.getSpeed());
    currentServiceStatus.setStep(btService.getStep());
    currentServiceStatus.setStyle(btService.getStyle());
}

void updateAllSecondaries()
{
    for (uint i = 0; i < allSecondaries.size(); i++)
    {
        allSecondaries[i]->setBrightness(currentServiceStatus.getBrightness());
        allSecondaries[i]->setSpeed(currentServiceStatus.getSpeed());
        allSecondaries[i]->setStep(currentServiceStatus.getStep());
        allSecondaries[i]->setStyle(currentServiceStatus.getStyle());
        allSecondaries[i]->setPattern(currentServiceStatus.getPattern());
    }
    ulong timestamp = millis();
    for (uint i = 0; i < allSecondaries.size(); i++)
    {
        allSecondaries[i]->updateSyncData(timestamp);
    }
}

void setManualStyle(uint style)
{
    // These style settings will need to change if the ordering of
    // styles/patterns changes.
    switch (style)
    {
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
    case 3:
        // Rainbow
        currentServiceStatus.setBrightness(10);
        currentServiceStatus.setPattern(1);
        currentServiceStatus.setSpeed(85);
        currentServiceStatus.setStep(95);
        currentServiceStatus.setStyle(0);
        break;
    case 7:
        // Rainbow random
        currentServiceStatus.setBrightness(10);
        currentServiceStatus.setPattern(6);
        currentServiceStatus.setSpeed(78);
        currentServiceStatus.setStep(55);
        currentServiceStatus.setStyle(0);
        break;
    default:
        // Rainbow - change?
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
        for (uint i = 0; i < allSecondaries.size(); i++)
        {
            Serial.print("Battery voltage for sign ");
            Serial.print(allSecondaries[i]->getSignOrder());
            Serial.print(": ");
            Serial.println(allSecondaries[i]->getBatteryVoltage());
        }
    }
}
