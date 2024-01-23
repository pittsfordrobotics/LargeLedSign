#include "main.h"

// TODO reminders:
// - Merge new pattern data into sign status (sign status has brightness and speed)
// - Setup predefined styles here instead of in Secondary
// - Clean up deprecated secondary characteristics from CommonPeripheral, move to new PrimaryPeripheral.
// - Have manual buttons detect a sequence of presses (ie, allow user to cylce though a bunch of styles).
// - (Still TBD) Enable a separate pattern for the logo

// Global variables
CommonPeripheral btService;
StatusDisplay display(TM1637_CLOCK, TM1637_DIO, TM1637_BRIGHTNESS);

std::vector<PushButton *> manualInputButtons;
std::vector<SecondaryClient *> allSecondaries;
ButtonConfiguration* buttonConfig;
ulong nextConnectionCheck = 0;
bool resetRequested = false;
bool shouldIgnoreLogo = false;
ulong loopCounter = 0;
ulong lastTelemetryTimestamp = 0;

SignStatus lastServiceStatus;
SignStatus currentServiceStatus;
int lastManualButtonPressed = -1;
int manualButtonSequenceNumber = 0;

PredefinedStyle defaultStyle = PredefinedStyle::getPredefinedStyle(PredefinedStyles::Pink_Solid);

void setup()
{
    Serial.begin(9600);
    delay(500);
    Serial.println("Starting...");

    std::vector<int> manualInputPins{MANUAL_INPUT_PINS};
    for (uint i = 0; i < manualInputPins.size(); i++)
    {
        manualInputButtons.push_back(new PushButton(manualInputPins[i], INPUT_PULLUP));
    }

    buttonConfig = new ButtonConfiguration(manualInputButtons.size());
    setupStyleLists();

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
    setManualStyle(defaultStyle);
    resetRequested = true;
    nextConnectionCheck = millis() + CONNECTION_CHECK_INTERVAL;
}

void loop()
{
    BLE.poll();
    display.update();

    if (btService.isConnected())
    {
        // Something is connected via BT.
        // Set the display to "--" to show something connected to us.
        // Display it as "temporary" since it's a low-priority message.
        display.displayTemporary(" --", 500);
    }
    else
    {
        checkSecondaryConnections();
    }

    updateInputButtons();
    readSettingsFromBLE();
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
    // First look for any long-presses that happened.
    // If button 1 (id 0) was long-pressed, display battery voltages for the clients.
    if (manualInputButtons[0]->wasPressed() && manualInputButtons[0]->lastPressType() == ButtonPressType::Long)
    {
        manualInputButtons[0]->clearPress();
        displayBatteryVoltages();
    }

    // If button 4 (id 3) was long-pressed, force-disconnect any BT clients.
    // If both buttons 3 and 4 (ids 2 and 3) were long-pressed, reconnect the clients.
    if (manualInputButtons[3]->wasPressed() && manualInputButtons[3]->lastPressType() == ButtonPressType::Long)
    {
        manualInputButtons[3]->clearPress();
        if (manualInputButtons[2]->wasPressed() && manualInputButtons[2]->lastPressType() == ButtonPressType::Long)
        {
            manualInputButtons[2]->clearPress();
            resetSecondaryConnections();
        }
        else
        {
            btService.disconnect();
        }
    }

    // Next look for any button presses that indicate style changes.
    // If the same button was pressed previously, cycle through the styles.
    for (uint i = 0; i < manualInputButtons.size(); i++)
    {
        if (manualInputButtons[i]->wasPressed() && manualInputButtons[i]->lastPressType() == ButtonPressType::Normal)
        {
            if ((int)i == lastManualButtonPressed)
            {
                // This button was pressed last time -- cycle through the sequence.
                manualButtonSequenceNumber++;
            }
            else
            {
                manualButtonSequenceNumber = 0;
            }

            // Get the vector corresponding to the button number (4 vectors; 1 per button),
            // then get the style by indexing into the vector.
            PredefinedStyle selectedStyle = buttonConfig->getStyleForButton(i, manualButtonSequenceNumber);
            setManualStyle(selectedStyle);
            manualInputButtons[i]->clearPress();
            lastManualButtonPressed = i;
        }
    }
}

void setManualStyle(PredefinedStyle style)
{
    currentServiceStatus.speed = style.getSpeed();
    currentServiceStatus.patternData = style.getPatternData();

    // Update the local BLE settings to reflect the new manual settings.
    btService.setSpeed(currentServiceStatus.speed);
    btService.setPatternData(currentServiceStatus.patternData);
    resetRequested = true;
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
    setManualStyle(defaultStyle);
    resetRequested = true;
}

void populateSecondaries()
{
    ulong scanTimeout = millis() + MAX_TOTAL_SCAN_TIME;

    display.setDisplay("C-0");
    // Continue to look for secondaries until we pass the timeout after finding at least one.
    while (millis() < scanTimeout || allSecondaries.size() == 0)
    {
        SecondaryClient *secondary = scanForSecondary();
        if (secondary->isValidClient())
        {
            // If the 'shouldIgnoreLogo' flag is set, don't add the logo as a secondary even if it was discovered.
            // We could use 'secondary->getServiceStatus().getSignConfigurationData().signType',
            // but it seems easier to rely on the fact that the local name ends with <position>-<type>.
            if (!shouldIgnoreLogo || !secondary->getLocalName().endsWith("-15"))
            {
                allSecondaries.push_back(secondary);
                display.setDisplay("C-" + String(allSecondaries.size()));

                // Reset timeout and continue looking for secondaries.
                scanTimeout = millis() + MAX_TOTAL_SCAN_TIME;
            }
            else
            {
                // Clean up the memory from the unused logo secondary.
                delete secondary;
            }
        }
        else
        {
            // Clean up the memory from the invalid peripheral.
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
    ulong scanTimeout = millis() + MAX_SCAN_TIME;

    Serial.print("Scanning for peripherals with uuid = ");
    Serial.println(BTCOMMON_SECONDARYCONTROLLER_UUID);
    bool allowDuplicateAdvertisements = true;
    BLE.scanForUuid(BTCOMMON_SECONDARYCONTROLLER_UUID, allowDuplicateAdvertisements);

    BLEDevice peripheral = BLE.available();
    while (!peripheral)
    {
        peripheral = BLE.available();

        if (millis() > scanTimeout)
        {
            Serial.println("Scan timed out.");
            // Return a dummy client - it'll be ignored and cleaned up.
            return new SecondaryClient(peripheral);
        }
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
        signConfigurations.push_back(status.signConfigurationData);
    }

    int numCols = 0;
    for (uint i = 0; i < numDigits; i++)
    {
        numCols += signConfigurations[i].columnCount;
    }

    int colsSoFar = 0;
    for (uint i = 0; i < numDigits; i++)
    {
        display.setDisplay("C--" + String(i + 1));
        SignOffsetData offsetData;
        offsetData.digitsToLeft = i;
        offsetData.digitsToRight = numDigits - i - 1;
        offsetData.columnsToLeft = colsSoFar;
        offsetData.columnsToRight = numCols - colsSoFar - signConfigurations[i].columnCount;
        colsSoFar += signConfigurations[i].columnCount;

        // Write the data back to the secondary
        allSecondaries[i]->setSignOffsetData(offsetData);
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

    SignStatus status = allSecondaries[0]->getSignStatus();
    btService.setBrightness(status.brightness);
    btService.setSpeed(status.speed);
    btService.setPatternData(status.patternData);
    btService.setColorPatternList(PatternFactory::getKnownColorPatterns());
    btService.setDisplayPatternList(PatternFactory::getKnownDisplayPatterns());

    Serial.println("Peripheral service started.");
    display.clear();
}

void readSettingsFromBLE()
{
    currentServiceStatus.brightness = btService.getBrightness();
    currentServiceStatus.speed = btService.getSpeed();
    currentServiceStatus.patternData = btService.getPatternData();
}

void updateAllSecondaries()
{
    for (uint i = 0; i < allSecondaries.size(); i++)
    {
        allSecondaries[i]->setBrightness(currentServiceStatus.brightness);
        allSecondaries[i]->setSpeed(currentServiceStatus.speed);
        allSecondaries[i]->setPatternData(currentServiceStatus.patternData);
    }
    ulong timestamp = millis();
    for (uint i = 0; i < allSecondaries.size(); i++)
    {
        allSecondaries[i]->updateSyncData(timestamp);
    }
}

void setupStyleLists()
{
    // Styles for button 1 (id 0)
    buttonConfig->addStyleToList(0, PredefinedStyles::Pink_Solid);

    // Styles for button 2 (id 1)
    buttonConfig->addStyleToList(1, PredefinedStyles::RedPink_Right);
    buttonConfig->addStyleToList(1, PredefinedStyles::RedPink_CenterOut);

    // Styles for button 3 (id 2)
    buttonConfig->addStyleToList(1, PredefinedStyles::BluePink_Right);
    buttonConfig->addStyleToList(1, PredefinedStyles::BluePink_CenterOut);

    // Styles for button 4 (id 3)
    buttonConfig->addStyleToList(1, PredefinedStyles::Rainbow_Right);
    buttonConfig->addStyleToList(1, PredefinedStyles::Rainbow_Random);
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