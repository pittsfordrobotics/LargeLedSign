#include "main.h"

std::vector<int> orderSelectorPins{ORDER_SELECTOR_PINS};     // Tells the controller which digit it's controlling (only the first pin is used so far)
std::vector<int> typeSelectorPins{STYLE_TYPE_SELECTOR_PINS}; // Tells the controller which digit it's controlling (only the first pin is used so far)

// Main BLE service wrapper
SecondaryPeripheral btService;

// Pixel and color data
PixelBuffer pixelBuffer(DATA_OUT);
LightStyle* currentLightStyle;
std::vector<LightStyle *> lightStyles; // ****

// Settings that are updated via bluetooth
byte currentBrightness = DEFAULTBRIGHTNESS;
byte newBrightness = DEFAULTBRIGHTNESS;
byte currentStyle = -1; // Force the style to "change" on the first iteration.
byte newStyle = DEFAULTSTYLE; // ****
byte currentSpeed = DEFAULTSPEED;
byte newSpeed = DEFAULTSPEED;
byte currentStep = DEFAULTSTEP; // ****
byte newStep = DEFAULTSTEP; // ****
byte currentPattern = DEFAULTPATTERN; // ****
byte newPattern = DEFAULTPATTERN; // ****
ulong currentSyncData = 0;
ulong newSyncData = 0;
SignOffsetData currentOffsetData;
byte signType;
byte signPosition;
PatternData newPatternData; // Is there a need for "new" vs "current"?

// Other internal state
int loopCounter = 0;                      // Records the number of times the main loop ran since the last timing calculation.
unsigned long lastTelemetryTimestamp = 0; // The last time debug information was emitted.
byte inLowPowerMode = false;              // Indicates the system should be in "low power" mode. This should be a boolean, but there are no bool types.

// Main entry point for the program --
// This is run once at startup.
void setup()
{
    // Delay for debugging
    delay(INITIALDELAY);
    Serial.begin(9600);
    Serial.println("Starting...");
    lastTelemetryTimestamp = millis();

    // Initialize components
    initializeIO();
    signType = getSignType();
    signPosition = getSignPosition();
    initializeLightStyles();

    pixelBuffer.initialize(signType);
    pixelBuffer.setBrightness(DEFAULTBRIGHTNESS);

    startBLE();

    // Setup the default pattern to show prior to any BT connections
    newPatternData.colorPattern = ColorPattern::SingleColor;
    newPatternData.displayPattern = DisplayPattern::Solid;
    // TODO: put the common colors in a helper class.
    newPatternData.color1 = Adafruit_NeoPixel::Color(230, 22, 161); // Pink
    // TODO: if we use "current" vs "new", the style creation and reset can be done in the updateLEDs method automatically.
    currentLightStyle = createLightStyleForNewPatternData();
    currentLightStyle->reset();
    btService.setPatternData(newPatternData);
}

// Main loop --
// This metod is called continously.
void loop()
{
    BLE.poll();
    emitTelemetry();
    // checkForLowPowerState();

    if (inLowPowerMode)
    {
        // blink LEDs and exit.
        blinkLowPowerIndicator();
        return;
    }

    // See if any settings have been changed via BLE and apply them if necessary.
    readBleSettings();

    // Apply any updates that were received via BLE or manually
    updateLEDs();
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

    Serial.println("Initializing the analog input to monitor battery voltage.");
    pinMode(VOLTAGEINPUTPIN, INPUT);
}

// Set up the list of "known" light styles.
// These are the styles presented in the "Styles" list in the phone app.
void initializeLightStyles()
{
    Serial.println("Initializing light styles");
    lightStyles.push_back(new RainbowStyle("Rainbow", &pixelBuffer));
    uint32_t pink = Adafruit_NeoPixel::Color(230, 22, 161);
    uint32_t red = Adafruit_NeoPixel::Color(255, 0, 0);
    uint32_t blue = Adafruit_NeoPixel::Color(0, 0, 255);
    // uint32_t white = Adafruit_NeoPixel::Color(255, 255, 255);
    uint32_t orange = Adafruit_NeoPixel::Color(255, 50, 0);
    lightStyles.push_back(new SingleColorStyle("Pink", pink, &pixelBuffer));
    lightStyles.push_back(new TwoColorStyle("Blue-Pink", blue, pink, &pixelBuffer));
    lightStyles.push_back(new SingleColorStyle("Blue", blue, &pixelBuffer));
    lightStyles.push_back(new TwoColorStyle("Red-Pink", red, pink, &pixelBuffer));
    lightStyles.push_back(new SingleColorStyle("Red", red, &pixelBuffer));
    lightStyles.push_back(new TwoColorStyle("Orange-Pink", orange, pink, &pixelBuffer));
    // lightStyles.push_back(new SingleColorStyle("White", white, &pixelBuffer));
}

byte getSignType()
{
    // Pull the pin low to indicate active.
    byte type = 0;
    for (uint i = 0; i < typeSelectorPins.size(); i++)
    {
        type = type << 1;
        if (digitalRead(typeSelectorPins.at(i)) == HIGH)
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
        if (digitalRead(orderSelectorPins.at(i)) == HIGH)
        {
            order++;
        }
    }

    if (order == 7)
    {
        // The circuit board for the logo doesn't have jumpers and will
        // read as 0b111 (7). It really should be position #2.
        order = 2;
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

    btService.initialize(BTCOMMON_SECONDARYCONTROLLER_UUID, localName);

    std::vector<String> styleNames;
    for (uint i = 0; i < lightStyles.size(); i++)
    {
        styleNames.push_back(lightStyles[i]->getName());
    }

    btService.setStyleNames(styleNames);
    btService.setPatternNames(LightStyle::knownPatterns);
    btService.setBrightness(DEFAULTBRIGHTNESS);
    btService.setStyle(DEFAULTSTYLE);
    btService.setSpeed(DEFAULTSPEED);
    btService.setPattern(DEFAULTPATTERN);
    btService.setStep(DEFAULTSTEP);
    btService.setSyncData(0);

    SignConfigurationData configData;
    configData.setSignType(signType);
    configData.setSignOrder(signPosition);
    configData.setColumnCount(pixelBuffer.getColumnCount());
    configData.setPixelCount(pixelBuffer.getPixelCount());

    btService.setSignConfigurationData(configData.getConfigurationString());
}

// Read the BLE settings to see if any have been changed.
void readBleSettings()
{
    String offsetString = btService.getSignOffsetData();
    SignOffsetData newOffsetData(offsetString);
    if (newOffsetData != currentOffsetData)
    {
        resetPixelBufferOffsets(newOffsetData);
        currentOffsetData = newOffsetData;
    }

    newPatternData = btService.getPatternData();
    newBrightness = btService.getBrightness();

    newSyncData = btService.getSyncData();
    // if (newSyncData > 0 && newSyncData == currentSyncData)
    // {
    //     // We're triggering changes based on the sync signal, but it hasn't changed.
    //     // Do nothing.
    //     return;
    // }

    // Check the range on the characteristic values.
    // If out of range, ignore the update and reset the BLE characteristic to the old value.
    // newStyle = btService.getStyle();
    // if (!isInRange(newStyle, 0, lightStyles.size() - 1))
    // {
    //     btService.setStyle(currentStyle);
    //     newStyle = currentStyle;
    // }

    newSpeed = btService.getSpeed();
    if (!isInRange(newSpeed, 1, 100))
    {
        btService.setSpeed(currentSpeed);
        newSpeed = currentSpeed;
    }

    // newStep = btService.getStep();
    // if (!isInRange(newStep, 1, 100))
    // {
    //     btService.setStep(currentStep);
    //     newStep = currentStep;
    // }

    // newPattern = btService.getPattern();
    // if (!isInRange(newPattern, 0, LightStyle::knownPatterns.size() - 1))
    // {
    //     btService.setPattern(currentPattern);
    //     newPattern = currentPattern;
    // }
}

// Determine if the give byte value is between (or equal to) the min and max values.
byte isInRange(byte value, byte minValue, byte maxValue)
{
    return (value >= minValue && value <= maxValue);
}

void blinkLowPowerIndicator()
{
    // Turn all LEDs off except for the first one, which will blink red.
    pixelBuffer.clearBuffer();
    pixelBuffer.displayPixels();
    delay(500);

    pixelBuffer.setPixel(0, Adafruit_NeoPixel::Color(255, 0, 0));
    pixelBuffer.displayPixels();
    delay(500);
}

// Set the style properties (speed, step, pattern, etc) if any have changed,
// and call the current style class to update the display.
void updateLEDs()
{
    if (newBrightness != currentBrightness)
    {
        pixelBuffer.setBrightness(newBrightness);
        currentBrightness = newBrightness;
    }

    int shouldResetStyle = false;
    if (newSyncData > 0 && newSyncData != currentSyncData)
    {
        // Sync data changed -- force a refresh even if nothing else changed.
        Serial.print("New sync data received: ");
        Serial.println(newSyncData);
        shouldResetStyle = true;
        currentSyncData = newSyncData;
    }

    if (shouldResetStyle)
    {
        if (currentLightStyle)
        {
            delete currentLightStyle;
        }

        currentLightStyle = createLightStyleForNewPatternData();
        currentLightStyle->setSpeed(newSpeed);
        currentLightStyle->reset();
    }

    // The current style shouldn't ever be null here, but check anyways.
    if (currentLightStyle)
    {
        currentLightStyle->update();
    }

    pixelBuffer.displayPixels();
}

LightStyle* createLightStyleForNewPatternData()
{
    LightStyle* newStyle;

    switch (newPatternData.colorPattern)
    {
        case ColorPattern::SingleColor:
            newStyle = new SingleColorStyle("singlecolor", newPatternData.color1, &pixelBuffer);
            break;
        case ColorPattern::TwoColor:
            newStyle = new TwoColorStyle("twocolor", newPatternData.color1, newPatternData.color2, &pixelBuffer);
            newStyle->setStep(newPatternData.param1);
            newStyle->setPattern(static_cast<byte>(newPatternData.displayPattern));
            break;
        case ColorPattern::Rainbow:
            newStyle = new RainbowStyle("rainbow", &pixelBuffer);
            newStyle->setStep(newPatternData.param1);
            newStyle->setPattern(static_cast<byte>(newPatternData.displayPattern));
            break;
        default:
            // unknown - use blank for now
            newStyle = new SingleColorStyle("blank", 0, &pixelBuffer);
    }

    return newStyle;
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
            // Enter low power mode. Disable LEDs and BLE.
            // For the LEDs, set brightness to 0 and call display to turn them off.
            // (This keeps the pixel buffer intact so we can resume where we left off when power returns.)
            pixelBuffer.setBrightness(0);
            pixelBuffer.displayPixels();
            // btService.stop();
            inLowPowerMode = true;
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
            // Exit low power mode.  Re-enable BLE.
            // btService.resume();
            pixelBuffer.setBrightness(currentBrightness);
            inLowPowerMode = false;
        }
    }
}

// Read the analog input from the "voltage input" pin and calculate the batter voltage.
float getCalculatedBatteryVoltage()
{
    // The analog input ranges from 0 (0V) to 1024 (3.3V), resulting in 0.00322 Volts per "tick".
    // The battery voltage passes through a voltage divider such that the voltage at the input
    // is 1/3 of the actual battery voltage.
    float rawLevel = getVoltageInputLevel();
    return rawLevel * 3 * 3.3 / 1024;
}

// Read the raw value from the "voltage input" pin.
int getVoltageInputLevel()
{
    return analogRead(VOLTAGEINPUTPIN);
}

// Calculate loop timing information and emit the current battery voltage level.
void emitTelemetry()
{
    loopCounter++;
    unsigned long timestamp = millis();

    if (timestamp > lastTelemetryTimestamp + TELEMETRYINTERVAL)
    {
        // Calculate loop timing data
        unsigned long diff = timestamp - lastTelemetryTimestamp;
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
    }
}

void indicateBleFailure()
{
    // Infinite loop here -- we can't continue with the rest of the service, so never return.
    Serial.println("Could not start BLE service!");
    while (true)
    {
        // Turn all LEDs off except for the first one, which will blink red/blue.
        pixelBuffer.setBrightness(255);
        pixelBuffer.clearBuffer();
        pixelBuffer.setPixel(0, Adafruit_NeoPixel::Color(0, 0, 255));
        pixelBuffer.displayPixels();
        delay(500);

        pixelBuffer.setPixel(0, Adafruit_NeoPixel::Color(255, 0, 0));
        pixelBuffer.displayPixels();
        delay(500);
    }
}

void resetPixelBufferOffsets(SignOffsetData offsetData)
{
    if (offsetData.getDigitsToLeft() >= 0)
    {
        pixelBuffer.setDigitsToLeft(offsetData.getDigitsToLeft());
    }
    if (offsetData.getDigitsToRight() >= 0)
    {
        pixelBuffer.setDigitsToRight(offsetData.getDigitsToRight());
    }
    if (offsetData.getColumnsToLeft() >= 0)
    {
        pixelBuffer.setColsToLeft(offsetData.getColumnsToLeft());
    }
    if (offsetData.getColumnsToRight() >= 0)
    {
        pixelBuffer.setColsToRight(offsetData.getColumnsToRight());
    }
}
