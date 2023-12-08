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

std::vector<ManualButton *> manualInputButtons;
std::vector<SecondaryClient *> allSecondaries;
ulong nextConnectionCheck = 0;
bool resetRequested = false;
bool shouldIgnoreLogo = false;
ulong loopCounter = 0;
ulong lastTelemetryTimestamp = 0;

// TODO: Merge the pattern data into the sign status
SignStatus lastServiceStatus;
SignStatus currentServiceStatus;

const ulong Pink = color(230, 22, 161);
const ulong Red = color(255, 0, 0);
const ulong Blue = color(0, 0, 255);
const ulong Orange = color(255, 50, 0);

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
    //readSettingsFromBLE();
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
    btService.setSpeed(status.getSpeed());
    btService.setPatternData(status.getPatternData());

    Serial.println("Peripheral service started.");
    display.clear();
}

void readSettingsFromBLE()
{
    currentServiceStatus.setBrightness(btService.getBrightness());
    currentServiceStatus.setSpeed(btService.getSpeed());
    currentServiceStatus.setPatternData(btService.getPatternData());
}

void updateAllSecondaries()
{
    for (uint i = 0; i < allSecondaries.size(); i++)
    {
        allSecondaries[i]->setBrightness(currentServiceStatus.getBrightness());
        allSecondaries[i]->setSpeed(currentServiceStatus.getSpeed());
        allSecondaries[i]->setPatternData(currentServiceStatus.getPatternData());
    }
    ulong timestamp = millis();
    for (uint i = 0; i < allSecondaries.size(); i++)
    {
        allSecondaries[i]->updateSyncData(timestamp);
    }
}

void setManualStyle(uint style)
{
    PatternData pattern;

    switch (style)
    {
        case 0:
            // Solid Pink
            currentServiceStatus.setBrightness(10);
            currentServiceStatus.setSpeed(1);
            pattern.colorPattern = ColorPattern::SingleColor;
            pattern.displayPattern = DisplayPattern::Solid;
            pattern.color1 = Pink;
            // old
            // currentServiceStatus.setPattern(0);
            // currentServiceStatus.setStep(1);
            // currentServiceStatus.setStyle(1);
            break;
        case 1:
            // Red-Pink
            currentServiceStatus.setBrightness(10);
            currentServiceStatus.setSpeed(25);
            pattern.colorPattern = ColorPattern::TwoColor;
            pattern.displayPattern = DisplayPattern::Right;
            pattern.color1 = Red;
            pattern.color2 = Pink;
            pattern.param1 = 25;
            // old
            // currentServiceStatus.setPattern(1);
            // currentServiceStatus.setStep(25);
            // currentServiceStatus.setStyle(4);
            break;
        case 2:
            // Blue-Pink
            currentServiceStatus.setBrightness(10);
            currentServiceStatus.setSpeed(25);
            pattern.colorPattern = ColorPattern::TwoColor;
            pattern.displayPattern = DisplayPattern::Right;
            pattern.color1 = Blue;
            pattern.color2 = Pink;
            pattern.param1 = 25;
            // old
            // currentServiceStatus.setPattern(1);
            // currentServiceStatus.setStep(25);
            // currentServiceStatus.setStyle(2);
            break;
        case 3:
            // Rainbow
            currentServiceStatus.setBrightness(10);
            currentServiceStatus.setSpeed(85);
            pattern.colorPattern = ColorPattern::Rainbow;
            pattern.displayPattern = DisplayPattern::Right;
            pattern.param1 = 95;
            // old
            // currentServiceStatus.setPattern(1);
            // currentServiceStatus.setStep(95);
            // currentServiceStatus.setStyle(0);
            break;
        case 7:
            // Rainbow random
            currentServiceStatus.setBrightness(10);
            currentServiceStatus.setSpeed(78);
            pattern.colorPattern = ColorPattern::Rainbow;
            pattern.displayPattern = DisplayPattern::Random;
            pattern.param1 = 95;
            // old
            // currentServiceStatus.setPattern(6);
            // currentServiceStatus.setStep(55);
            // currentServiceStatus.setStyle(0);
            break;
        default:
            // Rainbow - change?
            currentServiceStatus.setBrightness(10);
            currentServiceStatus.setSpeed(1);
            pattern.colorPattern = ColorPattern::SingleColor;
            pattern.displayPattern = DisplayPattern::Solid;
            pattern.color1 = Pink;
            // old
            // currentServiceStatus.setPattern(1);
            // currentServiceStatus.setStep(95);
            // currentServiceStatus.setStyle(0);
    }

    currentServiceStatus.setPatternData(pattern);

    // Update the local BLE settings to reflect the new manual settings.
    btService.setBrightness(currentServiceStatus.getBrightness());
    btService.setSpeed(currentServiceStatus.getSpeed());
    btService.setPatternData(pattern);

    // Set back the old characteristics so the next read won't think something changed.
    // btService.setPattern(currentServiceStatus.getPattern());
    // btService.setStyle(currentServiceStatus.getStyle());
    // btService.setStep(currentServiceStatus.getStep());
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

ulong color(byte red, byte green, byte blue)
{
    // Taken from the Adafruit_Neopixel::Color method.
    // I didn't want to add a dependency on that library for a single method, so copying it here.
    return ((ulong)red << 16) | ((ulong)green << 8) | blue;
}