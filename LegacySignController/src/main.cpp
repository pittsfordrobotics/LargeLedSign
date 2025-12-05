#include "main.h"

// Global variables
CommonPeripheral btService;
StatusDisplay* display;
std::vector<NeoPixelDisplay*>* neoPixelDisplays;
ButtonProcessor* buttonProcessor;
StyleConfiguration* styleConfiguration;
BatteryMonitorConfiguration batteryMonitorConfig;

bool isBluetoothEnabled = false;
ulong loopCounter = 0;
ulong lastTelemetryTimestamp = 0;
ulong ledLoopCounter = 0;
ulong lastLedTelemetryTimestamp = 0;
int sdCardChipSelectPin = SDCARD_CHIPSELECT;

int lastManualButtonPressed = -1;
int manualButtonSequenceNumber = 0;
byte inLowPowerMode = false;          // Indicates the system should be in "low power" mode. This should be a boolean, but there are no bool types.

// Settings that are updated via bluetooth
byte currentBrightness;
byte newBrightness;
byte currentSpeed;
byte newSpeed;
PatternData currentPatternData;
PatternData newPatternData;

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
    initializeDefaultStyleProperties(styleConfiguration->getDefaultStyle());

    display->setDisplay("---5");

    initializeBatteryMonitor(systemConfiguration->getBatteryMonitorConfiguration());

    display->setDisplay("---6");

    initializeBLEService(systemConfiguration->getBluetoothConfiguration());
    display->clear();
    isInitialized = true;
}

void loop()
{
    display->update();
    updateTelemetry();
    checkForLowPowerState();

    if (isBluetoothEnabled && btService.isConnected())
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
    String defaultConfigJson(defaultSystemConfigJson);
    defaultConfigJson.replace("[[BUTTONS]]", "");
    defaultConfigJson.replace("[[BT_UUID]]", "1221ca8d-4172-4946-bcd1-f9e4b40ba6b0");
    String localName = "3181 LED Controller ";
    localName.concat(signPosition);
    localName.concat("-");
    localName.concat(signType);
    defaultConfigJson.replace("[[BT_LOCALNAME]]", localName);
    String displayTypeName = "Display";
    displayTypeName.concat(signType);
    defaultConfigJson.replace("[[DISPLAYTYPENAME]]", displayTypeName);
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
    if (isBluetoothEnabled)
    {
        btService.setSpeed(newSpeed);
        btService.setPatternData(newPatternData);
    }
}

void initializeBLEService(BluetoothConfiguration& config)
{
    if (!config.isEnabled())
    {
        Serial.println("Bluetooth is disabled in configuration.");
        display->setDisplay("boff");
        delay(1500);
        return;
    }

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

    display->setDisplay("C  =");
    Serial.println("Setting up Peripheral service using common logic.");
    btService.initialize(config.getUuid(), config.getLocalName());

    // Set the various characteristics based on the defaults
    display->setDisplay("C ==");

    btService.setBrightness(currentBrightness);
    btService.setSpeed(currentSpeed);
    btService.setPatternData(currentPatternData);
    btService.setColorPatternList(PatternFactory::getKnownColorPatterns());
    btService.setDisplayPatternList(PatternFactory::getKnownDisplayPatterns());
    isBluetoothEnabled = true;
    Serial.println("Peripheral service started.");
}

void readSettingsFromBLE()
{
    if (!isBluetoothEnabled)
    {
        return;
    }

    BLE.poll();
    newBrightness = btService.getBrightness();
    newSpeed = btService.getSpeed();
    newPatternData = btService.getPatternData();
}

void initializeBatteryMonitor(BatteryMonitorConfiguration& config)
{
    batteryMonitorConfig = systemConfiguration->getBatteryMonitorConfiguration();
    
    if (batteryMonitorConfig.isEnabled())
    {
        pinMode(config.getAnalogInputPin(), INPUT);
    }
}

void configurePowerLed(PowerLedConfiguration& config)
{
    if (!config.isEnabled())
    {
        return;
    }
     
    pinMode(config.getGpioPin(), OUTPUT);
    digitalWrite(config.getGpioPin(), HIGH); // Turn on the power LED
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
        btService.emitBatteryVoltage(voltage);
        Serial.print("Battery monitor: ");
        Serial.print(batteryMonitorConfig.isEnabled() ? "Enabled" : "Disabled");
        Serial.print("; Raw input: ");
        Serial.print(rawLevel);
        Serial.print("; Calculated voltage: ");
        Serial.println(voltage);
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
    if (newBrightness != currentBrightness)
    {
        for (NeoPixelDisplay* neoPixelDisplay : *neoPixelDisplays)
        {
            neoPixelDisplay->setBrightness(newBrightness);
        }
        currentBrightness = newBrightness;
    }

    if (newSpeed != currentSpeed)
    {
        for (NeoPixelDisplay* neoPixelDisplay : *neoPixelDisplays)
        {
            neoPixelDisplay->setSpeed(newSpeed);
        }
        currentSpeed = newSpeed;
    }

    if (currentPatternData != newPatternData)
    {
        for (NeoPixelDisplay* neoPixelDisplay : *neoPixelDisplays)
        {
            DisplayPattern* newPattern = PatternFactory::createForPatternData(newPatternData);
            neoPixelDisplay->setDisplayPattern(newPattern);
            neoPixelDisplay->setSpeed(newSpeed);
            neoPixelDisplay->resetDisplay();
        }
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

    if (actionName == "disconnectBT")
    {
        btService.disconnect();
        return;
    }

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
