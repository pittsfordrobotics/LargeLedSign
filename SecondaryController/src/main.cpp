#include "main.h"

std::vector<int> orderSelectorPins{ORDER_SELECTOR_PINS};
std::vector<int> typeSelectorPins{STYLE_TYPE_SELECTOR_PINS};

// Main BLE service wrapper
SecondaryPeripheral btService;

// Pixel and color data
std::vector<DisplayConfiguration>* displayConfigs;
NeoPixelDisplay* neoPixelDisplay;
DisplayPattern *currentLightStyle;

// Settings that are updated via bluetooth
byte currentBrightness = DEFAULT_BRIGHTNESS;
byte newBrightness = DEFAULT_BRIGHTNESS;
byte currentSpeed = DEFAULT_SPEED;
byte newSpeed = DEFAULT_SPEED;
ulong currentSyncData = 0;
ulong newSyncData = 0;
SignOffsetData currentOffsetData;
byte signType;
byte signPosition;
PatternData currentPatternData;
PatternData newPatternData;
PredefinedStyle lowPowerStyle = PredefinedStyle::getPredefinedStyle(PredefinedStyles::LowPower);
PushButton powerButton(POWER_BUTTON_INPUT_PIN, INPUT_PULLUP);
PredefinedStyleList* predefinedStyleList = new PredefinedStyleList(1);
int buttonPressCount = 0;

// Other internal state
int loopCounter = 0;              // Records the number of times the main loop ran since the last timing calculation.
ulong lastTelemetryTimestamp = 0; // The last time debug information was emitted.
ulong lastBtTimestampUpdate = 0;  // The last time the 'timestamp' BT characteristic was updated.
byte inLowPowerMode = false;      // Indicates the system should be in "low power" mode. This should be a boolean, but there are no bool types.
ulong lastLedTelemetryTimestamp = 0; // The last time LED timing information was emitted.
int ledLoopCounter = 0;              // Records the number of times the LED update loop ran since the last timing calculation.
bool isOff = false;

// Used to signal the second core that the system is initialized.
volatile bool isInitialized = false;

void setup()
{
    // Delay for debugging
    delay(INITIALDELAY);
    Serial.begin(9600);
    Serial.println("Starting...");
    lastTelemetryTimestamp = millis();

    // Initialize components
    setupStyleList();
    initializeIO();
    turnOnPowerLed();
    signType = getSignType();
    signPosition = getSignPosition();

    
    byte defaultBrightness = DEFAULT_BRIGHTNESS;
    if (digitalRead(LOW_BRIGHTNESS_PIN) == LOW)
    {
        // The "local debug" pin is low, so assume we're testing using the test matrix.
        defaultBrightness = DEFAULT_BRIGHTNESS_LOW;
        signType = 0;
    }
    if (signType == PITSIGN_TYPE_ID) 
    {
        defaultBrightness = 255;
    }

    // TODO: Read the actual config from files!
    switch (signType)
    {
        case 1:
            displayConfigs = DisplayConfigFactory::createForDigit1();
            break;
        case 3:
            displayConfigs = DisplayConfigFactory::createForDigit3();
            break;
        case 8:
            displayConfigs = DisplayConfigFactory::createForDigit8();
            break;
        default:
            displayConfigs = DisplayConfigFactory::createForTestMatrix();
            break;
    }
    
    currentBrightness = defaultBrightness;
    newBrightness = defaultBrightness;
    neoPixelDisplay = new NeoPixelDisplay(displayConfigs->at(0));
    neoPixelDisplay->setBrightness(currentBrightness);

    // Setup the default pattern to show prior to any BT connections
    newPatternData.colorPattern = ColorPatternType::SingleColor;
    newPatternData.displayPattern = DisplayPatternType::Solid;
    newPatternData.color1 = Adafruit_NeoPixel::Color(230, 22, 161); // Pink
    currentLightStyle = PatternFactory::createForPatternData(newPatternData, nullptr);
    if (signType == PITSIGN_TYPE_ID) 
    {
        newPatternData.color1 = Adafruit_NeoPixel::Color(255,0,0);
        currentLightStyle = PatternFactory::createForPatternData(newPatternData, nullptr);
    }
    btService.setPatternData(newPatternData);
    neoPixelDisplay->setDisplayPattern(currentLightStyle);

    startBLE();

    isInitialized = true;
}

void loop()
{
    readInputButton();

    if (isOff)
    {
        // If we're supposed to be off, do nothing.
        delay(100);
        return;
    }

    BLE.poll();
    updateTelemetry();
    checkForLowPowerState();

    if (signType == PITSIGN_TYPE_ID && inLowPowerMode)
    {
        // The "pit" sign blinks a dedicated LED and doesn't use the main sign pixels.
        turnOffPowerLed();
        delay(500);
        turnOnPowerLed();
        delay(500);
        return;
    }

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
    updateLEDs();
    updateLedTelemetry();
}

// Initialize all input/output pins
void initializeIO()
{
    Serial.println("Initializing I/O pins.");
    for (uint i = 0; i < orderSelectorPins.size(); i++)
    {
        pinMode(orderSelectorPins.at(i), INPUT_PULLUP);
    }

    for (uint i = 0; i < typeSelectorPins.size(); i++)
    {
        pinMode(typeSelectorPins.at(i), INPUT_PULLUP);
    }

    pinMode(VOLTAGEINPUTPIN, INPUT);
    pinMode(LOW_BRIGHTNESS_PIN, INPUT_PULLUP);
    pinMode(POWER_INDICATOR_PIN, OUTPUT);
}

byte getSignType()
{
    // Pull the pin low to indicate active.
    byte type = 0;
    for (uint i = 0; i < typeSelectorPins.size(); i++)
    {
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
    // Pull the pin low to indicate active.
    byte order = 0;
    for (uint i = 0; i < orderSelectorPins.size(); i++)
    {
        order = order << 1;
        if (digitalRead(orderSelectorPins.at(i)) == LOW)
        {
            order++;
        }
    }

    return order;
}

// Set the initial BLE characteristic values and start the BLE service.
void startBLE()
{
    Serial.print("Initializing for sign in position ");
    Serial.print(signPosition);
    Serial.print(" and type ");
    Serial.println(signType);

    String localName = "3181 LED Controller ";
    localName.concat(signPosition);
    localName.concat("-");
    localName.concat(signType);

    // Start the actual BLE engine.
    if (!BLE.begin())
    {
        indicateBleFailure();
    }

    SignConfigurationData configData;
    configData.signType = signType;
    configData.signOrder = signPosition;
    configData.columnCount = displayConfigs->at(0).getNumberOfColumns();
    configData.pixelCount = displayConfigs->at(0).getNumberOfPixels();

    // If the sign "position" is 0, then we'll assume we're standalone.
    String uuid = signPosition == 0
                      ? BTCOMMON_PRIMARYCONTROLLER_UUID
                      : BTCOMMON_SECONDARYCONTROLLER_UUID;

    btService.initialize(uuid, localName);
    btService.setBrightness(currentBrightness);
    btService.setSpeed(currentSpeed);
    btService.setSignConfigurationData(configData);
    btService.setColorPatternList(PatternFactory::getKnownColorPatterns());
    btService.setDisplayPatternList(PatternFactory::getKnownDisplayPatterns());
}

// Read the BLE settings to see if any have been changed.
void readBleSettings()
{
    SignOffsetData newOffsetData = btService.getSignOffsetData();
    if (newOffsetData != currentOffsetData)
    {
        resetPixelBufferOffsets(newOffsetData);
        currentOffsetData = newOffsetData;
    }

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
        neoPixelDisplay->setBrightness(newBrightness);
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
        DisplayPattern* oldPattern = neoPixelDisplay->getDisplayPattern();
        DisplayPattern* newPattern = PatternFactory::createForPatternData(newPatternData, nullptr);
        newPattern->setSpeed(newSpeed);
        neoPixelDisplay->setDisplayPattern(newPattern);
        if (oldPattern)
        {
            delete oldPattern;
        }

        neoPixelDisplay->resetDisplay();

        currentPatternData = newPatternData;
    }

    neoPixelDisplay->updateDisplay();
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
    inLowPowerMode = true;

    // The pit sign has an explicit "low power" LED that we can use
    // instead of blinking the display pixels.
    if (signType == PITSIGN_TYPE_ID)
    {
        neoPixelDisplay->setBrightness(0);
        neoPixelDisplay->updateDisplay();
    }
    else
    {
        // Set up the "low power" display pattern.
        // The next call to updateLEDs will set this pattern for us.
        newBrightness = 255;
        newPatternData = lowPowerStyle.getPatternData();
        newSpeed = lowPowerStyle.getSpeed();
        newSyncData++;
    }
}

void exitLowPowerMode()
{
    inLowPowerMode = false;

    neoPixelDisplay->setBrightness(currentBrightness);
    newSyncData++;
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
    return analogRead(VOLTAGEINPUTPIN);
}

// Calculate loop timing information and emit the current battery voltage level.
void updateTelemetry()
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

void updateLedTelemetry()
{
    ledLoopCounter++;
    unsigned long timestamp = millis();

    if (timestamp > lastLedTelemetryTimestamp + TELEMETRYINTERVAL)
    {
        // Calculate loop timing data
        unsigned long diff = timestamp - lastLedTelemetryTimestamp;
        double timePerIteration = (double)diff / ledLoopCounter;
        Serial.print(ledLoopCounter);
        Serial.print(" LED iterations done in ");
        Serial.print(diff);
        Serial.print(" msec; avg msec per iteration: ");
        Serial.println(timePerIteration);
        lastLedTelemetryTimestamp = timestamp;
        ledLoopCounter = 0;
    }
}

void indicateBleFailure()
{
    // Infinite loop here -- we can't continue with the rest of the service, so never return.
    while (true)
    {
        Serial.println("Could not start BLE service!");
        // Turn all LEDs off except for the first one, which will blink red/blue.
        // pixelBuffer->setBrightness(255);
        // pixelBuffer->clearBuffer();
        // pixelBuffer->setPixel(0, Adafruit_NeoPixel::Color(0, 0, 255));
        // pixelBuffer->displayPixels();
        // TODO: extend NeoPixelDisplay to support this.
        delay(500);

        // pixelBuffer->setPixel(0, Adafruit_NeoPixel::Color(255, 0, 0));
        // pixelBuffer->displayPixels();
        //delay(500);
    }
}

void resetPixelBufferOffsets(SignOffsetData offsetData)
{
    // TODO:
    // This shouldn't be needed once the display configs
    // can be read via the SD Card.
    // In the mean time, the offsests won't do anything.
    // pixelBuffer->setDigitsToLeft(offsetData.digitsToLeft);
    // pixelBuffer->setDigitsToRight(offsetData.digitsToRight);
    // pixelBuffer->setColsToLeft(offsetData.columnsToLeft);
    // pixelBuffer->setColsToRight(offsetData.columnsToRight);
}

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
    powerButton.update();

    if (powerButton.wasPressed())
    {
        if (powerButton.lastPressType() == ButtonPressType::Long)
        {
            // Long-press: Update our power state.
            if (isOff)
            {
                // We're currently "off", so turn "on".
                // Attempting to restart the BT service by restarting advertising
                // nulls out the device's advertised name for some reason, even if
                // we set it explicitly before restarting advertising.
                // To get around this, just restart the entire system.
                rp2040.restart();
            }
            else
            {
                // We're currently "on", so turn "off".
                isOff = true;
                turnOffPowerLed();

                btService.disconnect();
                btService.stop();

                neoPixelDisplay->setBrightness(0);
                neoPixelDisplay->updateDisplay();
            }
        }
        else
        {
            // Normal press.  Cycle through sign styles.
            PredefinedStyle selectedStyle = predefinedStyleList->getStyle(0, buttonPressCount);
            newSpeed = selectedStyle.getSpeed();
            newPatternData = selectedStyle.getPatternData();

            // Update the local BLE settings to reflect the new manual settings.
            btService.setSpeed(newSpeed);
            btService.setPatternData(newPatternData);
            buttonPressCount++;
        }

        powerButton.clearPress();
    }
}

void setupStyleList()
{
    predefinedStyleList->addStyleToList(0, PredefinedStyles::BluePink_6inch);
    predefinedStyleList->addStyleToList(0, PredefinedStyles::RedPink_6inch);
    predefinedStyleList->addStyleToList(0, PredefinedStyles::Yellow_Solid);
    predefinedStyleList->addStyleToList(0, PredefinedStyles::Green_Solid);
    predefinedStyleList->addStyleToList(0, PredefinedStyles::Red_Solid);
}
 
