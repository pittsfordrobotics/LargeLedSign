#include "main.h"

//std::vector<int> orderSelectorPins{ORDER_SELECTOR_PINS};     // Tells the controller which digit it's controlling (only the first pin is used so far)
//std::vector<int> typeSelectorPins{STYLE_TYPE_SELECTOR_PINS}; // Tells the controller which digit it's controlling (only the first pin is used so far)

// Main BLE service wrapper
SecondaryPeripheral btService;

// Pixel and color data
std::vector<NeoPixelDisplay*> neoPixelDisplays;

// Settings that are updated via bluetooth
byte currentBrightness = DEFAULT_BRIGHTNESS;
byte newBrightness = DEFAULT_BRIGHTNESS;
byte currentSpeed = DEFAULT_SPEED;
byte newSpeed = DEFAULT_SPEED;
ulong currentSyncData = 0;
ulong newSyncData = 0;
PatternData currentPatternData;
PatternData newPatternData;
StyleConfiguration* styleConfig;
StyleDefinition lowPowerStyle = CommonStyles::LowPower();
//PushButton powerButton(POWER_BUTTON_INPUT_PIN, INPUT_PULLUP);
StyleList* manualStyleList = new StyleList(1);
int buttonPressCount = 0;

// Other internal state
int loopCounter = 0;              // Records the number of times the main loop ran since the last timing calculation.
ulong lastTelemetryTimestamp = 0; // The last time debug information was emitted.
ulong lastBtTimestampUpdate = 0;  // The last time the 'timestamp' BT characteristic was updated.
byte inLowPowerMode = false;      // Indicates the system should be in "low power" mode. This should be a boolean, but there are no bool types.

bool isOff = false;
volatile bool isInitialized = false;

// Main entry point for the program --
// This is run once at startup.
void setup()
{
    // Initialize SPI to use different pins than the default.
    //SPI.setSCK(0);
    //SPI.setMOSI(0);
    //SPI.setMISO(0);
    
    // Delay for debugging
    Serial.begin(9600);
    delay(INITIALDELAY);
    delay(3000);
    Serial.println("Starting...");
    lastTelemetryTimestamp = millis();

    // Initialize components
    initializeStyles();
    setupStyleList();
    initializeIO();
    turnOnPowerLed();

    configureLedDisplays();

    byte defaultBrightness = DEFAULT_BRIGHTNESS;
    currentBrightness = defaultBrightness;
    newBrightness = defaultBrightness;

    startBLE();

    // Setup the default pattern to show prior to any BT connections
    newPatternData = styleConfig->getDefaultStyle().getPatternData();
    currentPatternData = newPatternData;
    newSpeed = styleConfig->getDefaultStyle().getSpeed();
    currentSpeed = newSpeed;

    for (int i = 0; i < neoPixelDisplays.size(); i++)
    {
        DisplayPattern* defaultPattern = PatternFactory::createForPatternData(newPatternData);
        defaultPattern->setSpeed(newSpeed);
        neoPixelDisplays.at(i)->setDisplayPattern(PatternFactory::createForPatternData(newPatternData));
    }

    btService.setPatternData(newPatternData);

    isInitialized = true;
}

// Main loop --
// This method is called continously.
void loop()
{
    //readInputButton();
    if (isOff)
    {
        // If we're supposed to be off, do nothing.
        delay(100);
        return;
    }

    BLE.poll();
    //emitTelemetry();
    //checkForLowPowerState();

    // if (signType == PITSIGN_TYPE_ID && inLowPowerMode)
    // {
    //     // The "pit" sign blinks a dedicated LED and doesn't use the main sign pixels.
    //     turnOffPowerLed();
    //     delay(500);
    //     turnOnPowerLed();
    //     delay(500);
    //     return;
    // }

    if (!inLowPowerMode)
    {
        // See if any settings have been changed via BLE and apply them if necessary.
        readBleSettings();
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
    // Check for threading issues!!!
    updateLEDs();
}

// Initialize all input/output pins
void initializeIO()
{
    // Serial.println("Initializing I/O pins.");
    // for (uint i = 0; i < orderSelectorPins.size(); i++)
    // {
    //     pinMode(orderSelectorPins.at(i), INPUT_PULLUP);
    // }

    // for (uint i = 0; i < typeSelectorPins.size(); i++)
    // {
    //     pinMode(typeSelectorPins.at(i), INPUT_PULLUP);
    // }

    // pinMode(VOLTAGEINPUTPIN, INPUT);
    // pinMode(LOW_BRIGHTNESS_PIN, INPUT_PULLUP);
    // pinMode(POWER_INDICATOR_PIN, OUTPUT);
}

typedef void (*action)();

void doAction(action tapAction)
{
    tapAction();
}

void doActionNoTypeDef(void (*otherAction)())
{
    otherAction();
}

void actionTest()
{
    action a = [] {
        Serial.println("Action test");
    };

    doAction(a);

    doAction([] {
        Serial.println("Action test 2");
    });

    doActionNoTypeDef([] {
        Serial.println("Action test 3");
    });
}

void configureLedDisplays()
{
    // Configure LED display(s)
    const char* fileJson = getSdFileContents("displayconfiguration.json");
    std::vector<DisplayConfiguration>* displayConfigs = DisplayConfiguration::ParseJson(fileJson, strlen(fileJson));
    for (uint i = 0; i < displayConfigs->size(); i++)
    {
        NeoPixelDisplay* display = new NeoPixelDisplay(displayConfigs->at(i));
        display->setBrightness(displayConfigs->at(i).getDefaultBrightness());
        neoPixelDisplays.push_back(display);
    }

    Serial.print("Number of displays configured: ");
    Serial.println(neoPixelDisplays.size());

    if (fileJson) {
        delete fileJson;
    }
}

// Set the initial BLE characteristic values and start the BLE service.
void startBLE()
{
    const char* configFile = getSdFileContents("bluetooth.json");
    BluetoothConfig* config = BluetoothConfig::ParseJson(configFile, strlen(configFile));
    Serial.print("Starting BLE service with uuid: ");
    Serial.print(config->getUuid());
    Serial.print(" and local name: ");
    Serial.println(config->getLocalName());

    // Start the actual BLE engine.
    if (!BLE.begin())
    {
        Serial.println("Could not start BLE service!");
        indicateBleFailure();
    }

    btService.initialize(config->getUuid(), config->getLocalName());
    Serial.println("BLE service initialized.");
    btService.setBrightness(currentBrightness);
    btService.setSpeed(currentSpeed);
    btService.setColorPatternList(PatternFactory::getKnownColorPatterns());
    btService.setDisplayPatternList(PatternFactory::getKnownDisplayPatterns());

    // Clean up
    if (configFile)
    {
        delete configFile;
    }

    if (config)
    {
        delete config;
    }
}

// Read the BLE settings to see if any have been changed.
void readBleSettings()
{
    newPatternData = btService.getPatternData();
    newBrightness = btService.getBrightness();
    newSyncData = btService.getSyncData();
    newSpeed = btService.getSpeed();
}

// Set the style properties (speed, step, pattern, etc) if any have changed,
// and call the current style class to update the display.
void updateLEDs()
{
    if (newBrightness != currentBrightness)
    {
        for (uint i = 0; i < neoPixelDisplays.size(); i++)
        {
            neoPixelDisplays.at(i)->setBrightness(newBrightness);
        }

        currentBrightness = newBrightness;
    }

    int shouldResetStyle = false;
    if (newSyncData != currentSyncData)
    {
        // Sync data changed -- force a refresh even if nothing else changed.
        Serial.print("New sync data received: ");
        Serial.println(newSyncData);
        shouldResetStyle = true;
        currentSyncData = newSyncData;
    }

    if (currentSyncData == 0 && newSyncData == 0)
    {
        // We're not triggering on sync data, so just check if the pattern data changed.
        if (currentPatternData != newPatternData)
        {
            Serial.println("Detected a change in pattern data (no sync).");
            shouldResetStyle = true;
        }
    }

    if (shouldResetStyle)
    {
        for (int i = 0; i < neoPixelDisplays.size(); i++)
        {
            DisplayPattern* oldStyle = neoPixelDisplays.at(i)->getDisplayPattern();
            DisplayPattern* newStyle = PatternFactory::createForPatternData(newPatternData);
            newStyle->setSpeed(newSpeed);
            neoPixelDisplays.at(i)->setDisplayPattern(newStyle);
            if (oldStyle)
            {
                delete oldStyle;
            }

            neoPixelDisplays.at(i)->resetDisplay();
        }

        currentPatternData = newPatternData;
    }

    for (int i = 0; i < neoPixelDisplays.size(); i++)
    {
        neoPixelDisplays.at(i)->updateDisplay();
    }
}

// Check if the current battery voltage is too low to run the sign,
// or if the battery has been charged enough to restart operation.
void checkForLowPowerState()
{
    double currentVoltage = getCalculatedBatteryVoltage();

    // Check if the voltage is too low.
    if (currentVoltage < LOWPOWERTHRESHOLD)
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
            Serial.println(LOWPOWERTHRESHOLD);
            Serial.println("Entering low power mode.");
            enterLowPowerMode();
        }
    }

    // Check if the voltage is high enough for normal operation.
    if (currentVoltage > NORMALPOWERTHRESHOLD)
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
            Serial.println(NORMALPOWERTHRESHOLD);
            Serial.println("Exiting low power mode.");
            exitLowPowerMode();
        }
    }
}

void enterLowPowerMode()
{
    // inLowPowerMode = true;

    // // The pit sign has an explicit "low power" LED that we can use
    // // instead of blinking the display pixels.
    // if (signType == PITSIGN_TYPE_ID)
    // {
    //     pixelBuffer->setBrightness(0);
    //     pixelBuffer->displayPixels();
    //     pixelBuffer->stop();
    // }
    // else
    // {
    //     // Set up the "low power" display pattern.
    //     // The next call to updateLEDs will set this pattern for us.
    //     newBrightness = 255;
    //     newPatternData = lowPowerStyle.getPatternData();
    //     newSpeed = lowPowerStyle.getSpeed();
    //     newSyncData++;
    // }
}

void exitLowPowerMode()
{
    // inLowPowerMode = false;

    // pixelBuffer->resume();
    // pixelBuffer->setBrightness(currentBrightness);
    // newSyncData++;
}

// Read the analog input from the "voltage input" pin and calculate the batter voltage.
float getCalculatedBatteryVoltage()
{
    // The analog input ranges from 0 (0V) to 1024 (3.3V), resulting in 0.00322 Volts per "tick".
    // The battery voltage passes through a voltage divider so we have to multiply the input by
    // a scale factor to get the actual voltage.
    float rawLevel = getVoltageInputLevel();
    return rawLevel * VOLTAGEMULTIPLIER * 3.3 / 1024;
}

// Read the raw value from the "voltage input" pin.
int getVoltageInputLevel()
{
    //return analogRead(VOLTAGEINPUTPIN);
    return 0;
}

// Calculate loop timing information and emit the current battery voltage level.
void emitTelemetry()
{
    loopCounter++;
    ulong timestamp = millis();

    if (timestamp > lastBtTimestampUpdate + TIMESTAMPUPDATEINTERVAL)
    {
        btService.emitTimestamp(timestamp);
        lastBtTimestampUpdate = timestamp;
    }

    if (timestamp > lastTelemetryTimestamp + TELEMETRYINTERVAL)
    {
        // Calculate loop timing data
        ulong diff = timestamp - lastTelemetryTimestamp;
        double timePerIteration = (double)diff / loopCounter;
        Serial.print(loopCounter);
        Serial.print(" iterations (msec): ");
        Serial.print(diff);
        Serial.print("; avg per iteration (msec): ");
        Serial.println(timePerIteration);
        lastTelemetryTimestamp = timestamp;
        loopCounter = 0;

        // Output voltage info periodically
        int rawLevel = getVoltageInputLevel();
        float voltage = getCalculatedBatteryVoltage();

        // Emit battery voltage information on Bluetooth as well as Serial.
        btService.emitBatteryVoltage(voltage);
        Serial.print("Analog input: ");
        Serial.print(rawLevel);
        Serial.print("; calculated voltage: ");
        Serial.println(voltage);

        Serial.print("Bluetooth connected: ");
        Serial.println(btService.isConnected());
    }
}

void indicateBleFailure()
{
    // Infinite loop here -- we can't continue with the rest of the service, so never return.
    Serial.println("Could not start BLE service!");
    while (true)
    {
        // // Turn all LEDs off except for the first one, which will blink red/blue.
        // pixelBuffer->setBrightness(255);
        // pixelBuffer->clearBuffer();
        // pixelBuffer->setPixel(0, Adafruit_NeoPixel::Color(0, 0, 255));
        // pixelBuffer->displayPixels();
        // delay(500);

        // pixelBuffer->setPixel(0, Adafruit_NeoPixel::Color(255, 0, 0));
        // pixelBuffer->displayPixels();
        // delay(500);
    }
}

// void resetPixelBufferOffsets(SignOffsetData offsetData)
// {
//     // pixelBuffer->setDigitsToLeft(offsetData.digitsToLeft);
//     // pixelBuffer->setDigitsToRight(offsetData.digitsToRight);
//     // pixelBuffer->setColsToLeft(offsetData.columnsToLeft);
//     // pixelBuffer->setColsToRight(offsetData.columnsToRight);
// }

void turnOnPowerLed()
{
    digitalWrite(POWER_INDICATOR_PIN, HIGH);
}

void turnOffPowerLed()
{
    digitalWrite(POWER_INDICATOR_PIN, LOW);
}

void readInputButton()
{
    // powerButton.update();

    // if (powerButton.wasPressed())
    // {
    //     if (powerButton.lastPressType() == ButtonPressType::Long)
    //     {
    //         // Long-press: Update our power state.
    //         if (isOff)
    //         {
    //             // We're currently "off", so turn "on".
    //             // Attempting to restart the BT service by restarting advertising
    //             // nulls out the device's advertised name for some reason, even if
    //             // we set it explicitly before restarting advertising.
    //             // To get around this, just restart the entire system.
    //             // NVIC_SystemReset(); **Doesn't exist for new platform!
    //             // rp2040.restart();
    //         }
    //         else
    //         {
    //             // We're currently "on", so turn "off".
    //             isOff = true;
    //             turnOffPowerLed();

    //             btService.disconnect();
    //             btService.stop();

    //             pixelBuffer->setBrightness(0);
    //             pixelBuffer->displayPixels();
    //             pixelBuffer->stop();
    //         }
    //     }
    //     else
    //     {
    //         // Normal press.  Cycle through sign styles.
    //         StyleDefinition selectedStyle = manualStyleList->getStyle(0, buttonPressCount);
    //         newSpeed = selectedStyle.getSpeed();
    //         newPatternData = selectedStyle.getPatternData();

    //         // Update the local BLE settings to reflect the new manual settings.
    //         btService.setSpeed(newSpeed);
    //         btService.setPatternData(newPatternData);
    //         buttonPressCount++;
    //     }

    //     powerButton.clearPress();
    // }
}

void setupStyleList()
{
    manualStyleList->addStyleToList(0, CommonStyles::TwoColorDigit(Colors::Blue, Colors::Pink, 1, 1, 9));
    manualStyleList->addStyleToList(0, CommonStyles::TwoColorDigit(Colors::Red, Colors::Pink, 1, 1, 9));
    manualStyleList->addStyleToList(0, CommonStyles::SolidColor(Colors::Yellow));
    manualStyleList->addStyleToList(0, CommonStyles::SolidColor(Colors::Green));
    manualStyleList->addStyleToList(0, CommonStyles::SolidColor(Colors::Red));
}

void initializeStyles()
{
    const char* styleConfigContents = getSdFileContents("displayStyles.json");
    styleConfig = StyleConfiguration::ParseJson(styleConfigContents, strlen(styleConfigContents));
    Serial.print("Number of styles loaded: ");
    Serial.println(styleConfig->getStyles().size());

    if (styleConfigContents)
    {
        delete styleConfigContents;
    }
}
 
const char* getSdFileContents(String filename)
{
    Serial.println("Initializing SD card...");
    if (!SD.begin(SDCARD_CHIPSELECT))
    {
        Serial.println("SD card initialization failed!");
        return "";
    }

    Serial.println("Reading file '" + filename + "' from SD card.");
    File displayConfigFile = SD.open(filename);

    if (!displayConfigFile)
    {
        Serial.println("Could not open file!");
        return "";
    }

    size_t fileSize = displayConfigFile.size();

    Serial.println("Reading file.");
    char* fileContents = new char[fileSize + 1];
    displayConfigFile.readBytes(fileContents, fileSize);

    Serial.println("File read successfully.");

    displayConfigFile.close();
    SD.end();

    return fileContents;
}