#include "main.h"

// Global variables
CommonPeripheral* blePeripheralService = nullptr;
StatusDisplay* display = nullptr;
std::vector<NeoPixelDisplay*>* neoPixelDisplays = nullptr;
ButtonProcessor* buttonProcessor = nullptr;
StyleConfiguration* styleConfiguration = nullptr;
BatteryMonitorConfiguration batteryMonitorConfig;
PowerLedConfiguration powerLedConfig;
BluetoothConfiguration bluetoothConfig;

ulong loopCounter = 0;
ulong lastTelemetryTimestamp = 0;
ulong ledLoopCounter = 0;
ulong lastLedTelemetryTimestamp = 0;
int sdCardChipSelectPin = SDCARD_CHIPSELECT;

int lastManualButtonPressed = -1;
int manualButtonSequenceNumber = 0;
bool inLowPowerMode = false;

// Settings that are updated via bluetooth
byte currentBrightness;
byte newBrightness;
byte currentSpeed;
byte newSpeed;
PatternData currentPatternData;
PatternData newPatternData;
ulong currentSyncData = 0;
ulong newSyncData = 0;
SignOffsetData currentOffsetData;

std::vector<SecondaryClient *> allSecondaries;

volatile bool isInitialized = false;
SystemConfiguration* systemConfiguration;

void setup()
{
    Serial.begin(9600);
    delay(INITIAL_DELAY);
    Serial.println("Starting...");

    systemConfiguration = readSystemConfiguration();
    if (!systemConfiguration->isValid())
    {
        // Invalid config.  Not much we can do.
        while (true)
        {
            Serial.println("System configuration is invalid!");
            delay(1000);
        }
    }

    display = createStatusDisplay(systemConfiguration->getTm1637DisplayConfiguration());

    display->setDisplay("---1");
    buttonProcessor = systemConfiguration->getButtonProcessor();
    buttonProcessor->setActionProcessor(processButtonAction);

    display->setDisplay("---2");
    neoPixelDisplays = createNeoPixelDisplays(systemConfiguration->getDisplayConfigurationFile());

    display->setDisplay("---3");
    styleConfiguration = readStyleConfiguration(systemConfiguration->getStyleConfigurationFile());

    display->setDisplay("---4");
    Serial.println("---4");
    initializeDefaultStyleProperties(styleConfiguration->getDefaultStyle());

    display->setDisplay("---5");
    Serial.println("---5");
    batteryMonitorConfig = systemConfiguration->getBatteryMonitorConfiguration();
    initializeBatteryMonitor();

    display->setDisplay("---6");
    Serial.println("---6");
    powerLedConfig = systemConfiguration->getPowerLedConfiguration();
    initializePowerLed();

    display->setDisplay("---7");
    Serial.println("---7");
    bluetoothConfig = systemConfiguration->getBluetoothConfiguration();
    if (bluetoothConfig.isEnabled() || bluetoothConfig.isProxyModeEnabled())
    {
        if (!BLE.begin())
        {
            Serial.println("BLE initialization failed!");
            display->setDisplay("E-1");
            while (true)
            {
                Serial.println("BLE initialization failed!");
                delay(1000);
            }
        }
    }

    display->setDisplay("---8");
    Serial.println("---8");
    if (bluetoothConfig.isProxyModeEnabled())
    {
        Serial.println("Populating secondary clients...");
        populateSecondaryClients();
        updateOffsetDataForSecondaryClients();
        // Need to grab default brightness from someplace.
        if (allSecondaries.size() > 0)
        {
            newBrightness = allSecondaries[0]->getSignStatus().brightness;
            currentBrightness = newBrightness;
        }
    }

    display->setDisplay("---9");
    Serial.println("---9");
    initializeBlePeripheralService();

    display->clear();
    isInitialized = true;
}

void loop()
{
    display->update();
    updateTelemetry();
    checkForLowPowerState();

    if (bluetoothConfig.isEnabled() && blePeripheralService->isConnected())
    {
        // Something is connected via BT.
        // Set the display to "--" to show something connected to us.
        // Display it as "temporary" since it's a low-priority message.
        display->displayTemporary(" --", 200);
    }

    if (!inLowPowerMode)
    {
        // Only process inputs if we're not in low power mode.
        readSettingsFromBLE();
        buttonProcessor->update();

        if (bluetoothConfig.isProxyModeEnabled())
        {
            updateAllSecondaries();
        }
    }
}

// Setup for the second core
void setup1()
{
    while(!isInitialized) {}
}

// Main loop for the second core
void loop1()
{
    if (neoPixelDisplays->size() > 0) {
        updateLEDs();
    }

    updateLedTelemetry();
}

SystemConfiguration* readSystemConfiguration()
{
    // Initialize the SD Card reader
    SPI.setSCK(SDCARD_SPI_CLOCK);
    SPI.setMOSI(SDCARD_SPI_COPI);
    SPI.setMISO(SDCARD_SPI_CIPO);
    sdCardChipSelectPin = SDCARD_CHIPSELECT;

    const char* fileJson = getSdFileContents("systemconfiguration.json");

    if (fileJson != nullptr)
    {
        Serial.println("Parsing system configuration file.");
        SystemConfiguration* sc = SystemConfiguration::ParseJson(
            fileJson, 
            strlen(fileJson), 
            [](int gpioPin) {return (GenericButton*)(new ArduinoPushButton(gpioPin, INPUT_PULLUP));});    
            
        delete[] fileJson;

        return sc;
    }

    Serial.println("Failed to read system configuration file from SD Card on primary pins.");
    Serial.println("Attempting to read configuration from alternate SD Card pins.");
    SPI.setSCK(SDCARD_ALT_SPI_CLOCK);
    SPI.setMOSI(SDCARD_ALT_SPI_COPI);
    SPI.setMISO(SDCARD_ALT_SPI_CIPO);
    sdCardChipSelectPin = SDCARD_ALT_CHIPSELECT;

    fileJson = getSdFileContents("systemconfiguration.json");

    if (fileJson != nullptr)
    {
        Serial.println("Parsing system configuration file.");
        SystemConfiguration* sc = SystemConfiguration::ParseJson(
            fileJson, 
            strlen(fileJson), 
            [](int gpioPin) {return (GenericButton*)(new ArduinoPushButton(gpioPin, INPUT_PULLUP));});    
            
        delete[] fileJson;

        return sc;
    }

    // Still can't read from the SD card.
    // See if we can create a default configuration based on the physical config.
    Serial.println("Failed to read system configuration file from SD Card.");
    Serial.println("Attempting to generate default system configuration.");
    
    byte signType = getSignType();
    byte signPosition = getSignPosition();
    
    // Legacy sign type with SD Card connected on "alt" pins
    // but no SD card present (for testing) gives Type=0, Position=2.
    // Shouldn't be an issue in practice.

    // TBD: what does Primary give?
    // Shouldn't be an issue in practice (???).

    // Secondaries should give correct type and position.
    // Assume if we got here, we're dealing with a Secondary or the pit sign.
    Serial.print("Detected sign type: ");
    Serial.println(signType);
    Serial.print("Detected sign position: ");
    Serial.println(signPosition);

    // For now, only support secondaries.
    // Worry about the pit sign if it ever reappears.
    String defaultConfigJson = String(copyString(defaultSystemConfigJsonForSecondaries, strlen(defaultSystemConfigJsonForSecondaries)));
    
    // The replace function seems to get very confused when the source string is present more than once.
    defaultConfigJson.replace("[[SIGNTYPE1]]", String(signType));
    defaultConfigJson.replace("[[SIGNTYPE2]]", String(signType));
    defaultConfigJson.replace("[[SIGNPOSITION]]", String(signPosition));
    // Secondaries don't have their own buttons, so don't bother configuring them.
    defaultConfigJson.replace("[[BUTTONS]]", "");
    // Secondaries don't have their own style configuration, since they're driven by the primary.
    defaultConfigJson.replace("[[STYLECONFIGTYPENAME]]", "none");

    return SystemConfiguration::ParseJson(
        defaultConfigJson.c_str(),
        defaultConfigJson.length(), 
        [](int gpioPin) {return (GenericButton*)(new ArduinoPushButton(gpioPin, INPUT_PULLUP));});
}

StatusDisplay* createStatusDisplay(Tm1637DisplayConfiguration& config)
{
    if (config.isEnabled())
    {
        return (StatusDisplay*)(new TM1637StatusDisplay(
            config.getClockGpioPin(),
            config.getDataGpioPin(),
            config.getBrightness()));
    }

    return (StatusDisplay*)(new NullStatusDisplay());
}

std::vector<NeoPixelDisplay*>* createNeoPixelDisplays(String displayConfigFile)
{
    std::vector<NeoPixelDisplay*>* displays = new std::vector<NeoPixelDisplay*>();
    const char* fileJson = getSdFileContents(displayConfigFile);
    
    if (!fileJson) {
        Serial.println("Failed to read display configuration file.");
        return displays;
    }
    
    Serial.println("Parsing display configuration file.");
    std::vector<DisplayConfiguration>* displayConfigs = DisplayConfiguration::ParseJson(
        fileJson,
        strlen(fileJson));

    delete[] fileJson;
    
    if (!displayConfigs || displayConfigs->size() == 0)
    {
        Serial.println("No display configurations found in file.");
        return displays;
    }

    for (size_t i = 0; i < displayConfigs->size(); i++)
    {
        displays->push_back(new NeoPixelDisplay(displayConfigs->at(i)));
    }

    return displays;
}

StyleConfiguration* readStyleConfiguration(String styleConfigFile)
{
    const char* fileJson = getSdFileContents(styleConfigFile);
    
    // If we can't read the style configuration file, return an empty style configuration.
    if (!fileJson) {
        Serial.println("Failed to read style configuration file.");
        return StyleConfiguration::ParseJson("", 0);
    }
    
    StyleConfiguration* styleConfiguration = StyleConfiguration::ParseJson(
        fileJson,
        strlen(fileJson));
        
    delete[] fileJson;

    return styleConfiguration;
}

void initializeDefaultStyleProperties(StyleDefinition& defaultStyleDefinition)
{
    // Setup the default display pattern for the sign,
    // and initialize the starting values for the pattern, speed, etc.

    // Set the default pattern data
    newPatternData = defaultStyleDefinition.getPatternData();
    currentPatternData = newPatternData;

    // Set default speed
    newSpeed = defaultStyleDefinition.getSpeed();
    currentSpeed = newSpeed;

    // Set default brightness
    newBrightness = 0;
    if (neoPixelDisplays->size() > 0)
    {
        newBrightness = neoPixelDisplays->at(0)->getBrightness();
    }

    currentBrightness = newBrightness;

    for (NeoPixelDisplay* neoPixelDisplay : *neoPixelDisplays)
    {
        DisplayPattern* initialPattern = PatternFactory::createForPatternData(newPatternData);
        initialPattern->setSpeed(newSpeed);
        neoPixelDisplay->setDisplayPattern(initialPattern);
    }
}

void setManualStyle(StyleDefinition styleDefinition)
{
    newSpeed = styleDefinition.getSpeed();
    newPatternData = styleDefinition.getPatternData();

    // Update the local BLE settings to reflect the new manual settings.
    if (bluetoothConfig.isEnabled())
    {
        blePeripheralService->setSpeed(newSpeed);
        blePeripheralService->setPatternData(newPatternData);
    }
}

void initializeBlePeripheralService()
{
    if (!bluetoothConfig.isEnabled())
    {
        Serial.println("Bluetooth is disabled in configuration.");
        display->setDisplay("boff");
        delay(1500);
        return;
    }

    display->setDisplay("C  =");
    if (bluetoothConfig.isSecondaryModeEnabled())
    {
        // Initialize the secondary peripheral service.
        Serial.println("Initializing secondary peripheral service.");
        blePeripheralService = new SecondaryPeripheral();
    }
    else
    {
        // Initialize the common peripheral service.
        Serial.println("Initializing common peripheral service.");
        blePeripheralService = new CommonPeripheral();
    }

    blePeripheralService->initialize(bluetoothConfig.getUuid(), bluetoothConfig.getLocalName());

    // Set the various characteristics based on the defaults
    display->setDisplay("C ==");

    blePeripheralService->setBrightness(currentBrightness);
    blePeripheralService->setSpeed(currentSpeed);
    blePeripheralService->setPatternData(currentPatternData);
    blePeripheralService->setColorPatternList(PatternFactory::getKnownColorPatterns());
    blePeripheralService->setDisplayPatternList(PatternFactory::getKnownDisplayPatterns());
    
    if (bluetoothConfig.isSecondaryModeEnabled())
    {
        SignConfigurationData signConfigData;
        if (neoPixelDisplays->size() > 0) {
            // The assumption here is that if we're in secondary mode, the configuration is read from the pins.
            signConfigData.signType = getSignType();
            signConfigData.signOrder = getSignPosition();
            
            // Not exactly sure how to handle multiple displays...
            signConfigData.columnCount = neoPixelDisplays->at(0)->getColumnCount();
            signConfigData.digitCount = neoPixelDisplays->at(0)->getDigitCount();
            signConfigData.pixelCount = neoPixelDisplays->at(0)->getPixelCount();
        }

        ((SecondaryPeripheral*)blePeripheralService)->setSignConfigurationData(signConfigData);
    }

    Serial.println("Peripheral service started.");
}

void readSettingsFromBLE()
{
    if (!bluetoothConfig.isEnabled())
    {
        return;
    }

    BLE.poll();
    if (bluetoothConfig.isSecondaryModeEnabled())
    {
        newSyncData = ((SecondaryPeripheral*)blePeripheralService)->getSyncData();

        SignOffsetData newOffsetData = ((SecondaryPeripheral*)blePeripheralService)->getSignOffsetData();
        if (newOffsetData != currentOffsetData && neoPixelDisplays->size() > 0)
        {
            // Secondary offsets don't really handle multiple displays well.
            // Just update the first one (should only be one).
            neoPixelDisplays->at(0)->setDigitsToLeft(newOffsetData.digitsToLeft);
            neoPixelDisplays->at(0)->setDigitsToRight(newOffsetData.digitsToRight);
            neoPixelDisplays->at(0)->setColumnsToLeft(newOffsetData.columnsToLeft);
            neoPixelDisplays->at(0)->setColumnsToRight(newOffsetData.columnsToRight);            
            currentOffsetData = newOffsetData;
        }
    }

    newBrightness = blePeripheralService->getBrightness();
    newSpeed = blePeripheralService->getSpeed();
    newPatternData = blePeripheralService->getPatternData();
}

void initializeBatteryMonitor()
{
    if (batteryMonitorConfig.isEnabled())
    {
        pinMode(batteryMonitorConfig.getAnalogInputPin(), INPUT);
    }
}

void initializePowerLed()
{
    if (powerLedConfig.isEnabled())
    {
        pinMode(powerLedConfig.getGpioPin(), OUTPUT);
        digitalWrite(powerLedConfig.getGpioPin(), HIGH); // Turn on the power LED
    }
}

// Read the analog input from the "voltage input" pin and calculate the battery voltage.
float getCalculatedBatteryVoltage()
{
    if (!batteryMonitorConfig.isEnabled())
    {
        return 0.0f;
    }

    // The analog input ranges from 0 (0V) to 1024 (3.3V), resulting in 0.00322 Volts per "tick".
    // The battery voltage passes through a voltage divider so we have to multiply the input by
    // a scale factor to get the actual voltage.
    float rawLevel = getRawVoltageInputLevel();
    return rawLevel * batteryMonitorConfig.getInputMultiplier() * 3.3 / 1024;
}

// Read the raw value from the "voltage input" pin.
int getRawVoltageInputLevel()
{
    if (!batteryMonitorConfig.isEnabled())
    {
        return 0;
    }

    return analogRead(batteryMonitorConfig.getAnalogInputPin());
}

void displayBatteryVoltage()
{
    double voltage = getCalculatedBatteryVoltage();
    display->displayTemporary(String(voltage, 2), 1500);
}

// Check if the current battery voltage is too low to run the sign,
// or if the battery has been charged enough to restart operation.
void checkForLowPowerState()
{
    if (!batteryMonitorConfig.isEnabled())
    {
        return;
    }

    double currentVoltage = getCalculatedBatteryVoltage();

    // Check if the voltage is too low.
    if (currentVoltage < batteryMonitorConfig.getVoltageToEnterLowPowerState())
    {
        if (inLowPowerMode)
        {
            // Already in low power mode - nothing to do.
        }
        else
        {
            Serial.print("Battery voltage is below threshold. Voltage: ");
            Serial.print(currentVoltage);
            Serial.print(", threshold: ");
            Serial.println(batteryMonitorConfig.getVoltageToEnterLowPowerState());
            Serial.println("Entering low power mode.");
            inLowPowerMode = true;

            // Set up the "low power" display pattern.
            // The next call to updateLEDs will set this pattern for us.
            PatternData lowPowerPattern;
            lowPowerPattern.colorPattern = ColorPatternType::SingleColor;
            lowPowerPattern.displayPattern = DisplayPatternType::LowPower;
            lowPowerPattern.color1 = 0xFF0000; // Red

            newBrightness = 255;
            newPatternData = lowPowerPattern;
            newSpeed = 1;
            newSyncData++;
        }
    }

    // Check if the voltage is high enough for normal operation.
    if (currentVoltage > batteryMonitorConfig.getVoltageToExitLowPowerState())
    {
        if (!inLowPowerMode)
        {
            // Not in low power mode - nothing to do.
        }
        else
        {
            Serial.print("Battery voltage is above normal threshold. Voltage: ");
            Serial.print(currentVoltage);
            Serial.print(", threshold: ");
            Serial.println(batteryMonitorConfig.getVoltageToExitLowPowerState());
            Serial.println("Exiting low power mode.");
            for (NeoPixelDisplay* neoPixelDisplay : *neoPixelDisplays)
            {
                neoPixelDisplay->setBrightness(currentBrightness);
            }
            inLowPowerMode = false;
            newSyncData++;
        }
    }
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

        // Output voltage info periodically
        int rawLevel = getRawVoltageInputLevel();
        float voltage = getCalculatedBatteryVoltage();

        // Emit battery voltage information on Bluetooth as well as Serial.
        if (bluetoothConfig.isEnabled())
        {
            blePeripheralService->emitBatteryVoltage(voltage);
        }

        Serial.print("Battery monitor: ");
        Serial.print(batteryMonitorConfig.isEnabled() ? "Enabled" : "Disabled");
        Serial.print("; Raw input: ");
        Serial.print(rawLevel);
        Serial.print("; Calculated voltage: ");
        Serial.println(voltage);

        if (bluetoothConfig.isEnabled())
        {
            Serial.print("Bluetooth connected: ");
            Serial.println(blePeripheralService->isConnected());
        }
    }
}

void updateLedTelemetry()
{
    ledLoopCounter++;
    unsigned long timestamp = millis();

    if (timestamp > lastLedTelemetryTimestamp + TELEMETRYINTERVAL)
    {
        if (neoPixelDisplays->size() == 0) {
            Serial.println("No LED displays configured.");
        }
        else
        {
            // Calculate loop timing data
            unsigned long diff = timestamp - lastLedTelemetryTimestamp;
            double timePerIteration = (double)diff / ledLoopCounter;
            Serial.print(ledLoopCounter);
            Serial.print(" LED iterations done in ");
            Serial.print(diff);
            Serial.print(" msec; avg msec per iteration: ");
            Serial.println(timePerIteration);
        }

        lastLedTelemetryTimestamp = timestamp;
        ledLoopCounter = 0;
    }
}

void updateLEDs()
{
    bool shouldUpdate = false;

    if (bluetoothConfig.isSecondaryModeEnabled())
    {
        // We're in secondary mode, so only update when the sync data changes.
        if (newSyncData != currentSyncData)
        {
            shouldUpdate = true;
            currentSyncData = newSyncData;
        }
    }
    else
    {
        // We're in standalone mode, so trigger on any changes.
        if (newBrightness != currentBrightness ||
            newSpeed != currentSpeed ||
            newPatternData != currentPatternData)
        {
            shouldUpdate = true;
        }
    }

    if (shouldUpdate)
    {
        for (NeoPixelDisplay* neoPixelDisplay : *neoPixelDisplays)
        {
            neoPixelDisplay->setBrightness(newBrightness);
            // If we're in secondary mode, force a reset of the display pattern even if the pattern didn't change.
            if (bluetoothConfig.isSecondaryModeEnabled() || currentPatternData != newPatternData)
            {
                DisplayPattern* newPattern = PatternFactory::createForPatternData(newPatternData);
                neoPixelDisplay->setDisplayPattern(newPattern);
                neoPixelDisplay->resetDisplay();
            }
            neoPixelDisplay->setSpeed(newSpeed);
        }
        currentBrightness = newBrightness;
        currentSpeed = newSpeed;
        currentPatternData = newPatternData;
    }

    for (NeoPixelDisplay* neoPixelDisplay : *neoPixelDisplays)
    {
        neoPixelDisplay->updateDisplay();
    }
}

void processButtonAction(int callerId, String actionName, std::vector<String> arguments)
{
    Serial.print("Processing button action for'");
    Serial.print(callerId);
    Serial.print("', action '");
    Serial.print(actionName);
    Serial.print("', argument count: ");
    Serial.println(arguments.size());

    if (actionName == "batteryVoltage")
    {
        displayBatteryVoltage();
        return;
    }

    if (actionName == "disconnectBT" && bluetoothConfig.isEnabled())
    {
        blePeripheralService->disconnect();
        return;
    }

    // TBD: logic for "secondaryBatteryVoltage" and possibly "resetSecondaries"

    if (actionName == "changeStyle")
    {
        if (arguments.size() < 1)
        {
            // Invalid number of arguments. Set to the default.
            setManualStyle(styleConfiguration->getDefaultStyle());
            return;
        }

        if (callerId == lastManualButtonPressed)
        {
            manualButtonSequenceNumber = (manualButtonSequenceNumber + 1) % arguments.size();
        }
        else
        {
            manualButtonSequenceNumber = 0;
        }

        lastManualButtonPressed = callerId;

        if (manualButtonSequenceNumber >= arguments.size())
        {
            // Invalid sequence number - reset to 0.
            Serial.println("Invalid manual button sequence number detected. Resetting to 0.");
            manualButtonSequenceNumber = 0;
        }

        if (arguments.size() == 0)
        {
            Serial.println("No styles defined in button action arguments.");
            return;
        }

        // Get styleName from argument list, based on the number of times the callerId was pressed.
        String styleName = arguments[manualButtonSequenceNumber];

        StyleDefinition styleDef = styleConfiguration->getDefaultStyle();
        for (StyleDefinition& def : styleConfiguration->getStyles())
        {
            if (def.getName().equalsIgnoreCase(styleName))
            {
                styleDef = def;
                break;
            }
        }

        setManualStyle(styleDef);
    }
}

const char* getSdFileContents(String filename)
{
    if (filename.startsWith("::") && filename.endsWith("::"))
    {
        // It's a built-in file.
        return readBuiltInFile(filename);
    }

    if (!SD.begin(sdCardChipSelectPin))
    {
        Serial.println("SD card initialization failed!");
        return nullptr;
    }

    if (!SD.exists(filename))
    {
        Serial.println("File does not exist on SD card: " + filename);
        SD.end();
        return nullptr;
    }
    Serial.println("Reading file '" + filename + "' from SD card.");
    File file = SD.open(filename);

    if (!file)
    {
        Serial.println("Could not open file!");
        SD.end();
        return nullptr;
    }

    size_t fileSize = file.size();

    Serial.println("Reading file.");
    char* fileContents = new char[fileSize + 1];
    file.readBytes(fileContents, fileSize);
    fileContents[fileSize] = '\0';  // Add null terminator

    Serial.println("File read successfully.");

    file.close();
    SD.end();

    return fileContents;
}

byte getSignType()
{
    // Check the "test" pin
    pinMode(TEST_MODE_PIN, INPUT_PULLUP);
    if (digitalRead(TEST_MODE_PIN) == LOW)
    {
        Serial.println("Test mode detected. Returning 255 for TestMatrix sign type.");
        return 255;
    }

    std::vector<int> typeSelectorPins = {STYLE_TYPE_SELECTOR_PINS};

    // Pull the pin low to indicate active.
    byte type = 0;
    for (uint i = 0; i < typeSelectorPins.size(); i++)
    {
        pinMode(typeSelectorPins.at(i), INPUT_PULLUP);
        type = type << 1;
        if (digitalRead(typeSelectorPins.at(i)) == LOW)
        {
            type++;
        }
    }

    return type;
}

byte getSignPosition()
{
    std::vector<int> orderSelectorPins = {ORDER_SELECTOR_PINS};

    // Pull the pin low to indicate active.
    byte order = 0;
    for (uint i = 0; i < orderSelectorPins.size(); i++)
    {
        order = order << 1;
        pinMode(orderSelectorPins.at(i), INPUT_PULLUP);
        if (digitalRead(orderSelectorPins.at(i)) == LOW)
        {
            order++;
        }
    }

    return order;
}

const char* readBuiltInFile(String filename)
{
    // "Read" Default display configs.
    Serial.println("'Reading' built-in file: " + filename);

    if (filename == "::Display1::")
    {
        return copyString(DisplayConfigFactory::getDigit1Json(), strlen(DisplayConfigFactory::getDigit1Json()));
    }

    if (filename == "::Display3::")
    {
        return copyString(DisplayConfigFactory::getDigit3Json(), strlen(DisplayConfigFactory::getDigit3Json()));
    }

    if (filename == "::Display8::")
    {
        return copyString(DisplayConfigFactory::getDigit8Json(), strlen(DisplayConfigFactory::getDigit8Json()));
    }

    if (filename == "::Display15::")
    {
        return copyString(DisplayConfigFactory::getLogoJson(), strlen(DisplayConfigFactory::getLogoJson()));
    }

    if (filename == "::Display255::")
    {
        return copyString(DisplayConfigFactory::getTestMatrixJson(), strlen(DisplayConfigFactory::getTestMatrixJson()));
    }

    // TODO (if the pit sign comes back):
    // ::Display10:: is the pit sign
    // ::PitSignStyle:: should be the default styles

    Serial.println("Built-in file not found. Returning null.");
    return nullptr;
}

const char* copyString(const char* source, size_t length)
{
    char* dest = new char[length + 1];
    strncpy(dest, source, length);
    dest[length] = '\0';
    return dest;
}

void populateSecondaryClients()
{
    ulong scanTimeout = millis() + MAX_TOTAL_SCAN_TIME;

    display->setDisplay("C-0");
    // Continue to look for secondaries until we pass the timeout after finding at least one.
    while (millis() < scanTimeout || allSecondaries.size() == 0)
    {
        SecondaryClient *secondary = scanForSecondaryClient();
        if (secondary->isValidClient())
        {
            allSecondaries.push_back(secondary);
            display->setDisplay("C-" + String(allSecondaries.size()));

            // Reset timeout and continue looking for secondaries.
            scanTimeout = millis() + MAX_TOTAL_SCAN_TIME;
        }
        else
        {
            // Clean up the memory from the invalid peripheral.
            delete secondary;
        }
    }

    // We found all we could in the time interval. Order them by position, lowest position first.
    std::sort(
            allSecondaries.begin(),
            allSecondaries.end(),
            [](SecondaryClient *&a, SecondaryClient *&b)
            { return a->getSignOrder() < b->getSignOrder(); });

    display->clear();
}

SecondaryClient *scanForSecondaryClient()
{
    ulong scanTimeout = millis() + MAX_SECONDARY_SCAN_TIME;

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
            BLE.stopScan();
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

void updateOffsetDataForSecondaryClients()
{
    display->setDisplay("C---");
    uint numSecondaries = allSecondaries.size();
    if (numSecondaries == 0)
    {
        // Not sure how we got here with no secondary clients.
        display->clear();
        return;
    }

    // Stash the sign config data for each secondary so we don't retrieve it every time.
    std::vector<SignConfigurationData> signConfigurations;
    for (uint i = 0; i < numSecondaries; i++)
    {
        SignStatus status = allSecondaries[i]->getSignStatus();
        signConfigurations.push_back(status.signConfigurationData);
    }

    int numCols = 0;
    for (uint i = 0; i < numSecondaries; i++)
    {
        numCols += signConfigurations[i].columnCount;
    }

    int colsSoFar = 0;
    for (uint i = 0; i < numSecondaries; i++)
    {
        display->setDisplay("C--" + String(i + 1));
        SignOffsetData offsetData;
        offsetData.digitsToLeft = i;
        offsetData.digitsToRight = numSecondaries - i - 1;
        offsetData.columnsToLeft = colsSoFar;
        offsetData.columnsToRight = numCols - colsSoFar - signConfigurations[i].columnCount;
        colsSoFar += signConfigurations[i].columnCount;

        // Write the data back to the secondary
        allSecondaries[i]->setSignOffsetData(offsetData);
    }

    display->clear();
}

void updateAllSecondaries()
{
    // NOTE:
    // After calling this method, the currentXXX variables will be updated to match the newXXX variables.
    // It's assumed that we won't be running "proxy" and be updating NeoPixel displays at the same time.
    if (allSecondaries.size() == 0)
    {
        return;
    }

    if (currentBrightness == newBrightness &&
        currentSpeed == newSpeed &&
        currentPatternData == newPatternData)
    {
        // No changes to propagate.
        return;
    }
    
    for (uint i = 0; i < allSecondaries.size(); i++)
    {
        allSecondaries[i]->setBrightness(newBrightness);
        allSecondaries[i]->setSpeed(newSpeed);
        allSecondaries[i]->setPatternData(newPatternData);
    }

    ulong timestamp = millis();
    for (uint i = 0; i < allSecondaries.size(); i++)
    {
        allSecondaries[i]->updateSyncData(timestamp);
    }

    currentBrightness = newBrightness;
    currentSpeed = newSpeed;
    currentPatternData = newPatternData;
}
