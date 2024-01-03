#include "main.h"

std::vector<int> orderSelectorPins{ORDER_SELECTOR_PINS};     // Tells the controller which digit it's controlling (only the first pin is used so far)
std::vector<int> typeSelectorPins{STYLE_TYPE_SELECTOR_PINS}; // Tells the controller which digit it's controlling (only the first pin is used so far)

// Main BLE service wrapper
SecondaryPeripheral btService;

// Pixel and color data
PixelBuffer pixelBuffer(DATA_OUT);
DisplayPattern* currentLightStyle;

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

// Other internal state
int loopCounter = 0;                      // Records the number of times the main loop ran since the last timing calculation.
ulong lastTelemetryTimestamp = 0;         // The last time debug information was emitted.
ulong lastBtTimestampUpdate = 0;          // The last time the 'timestamp' BT characteristic was updated.
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

    pixelBuffer.initialize(signType);

    byte defaultBrightness = DEFAULT_BRIGHTNESS;
    if (digitalRead(LOW_BRIGHTNESS_PIN) == LOW)
    {
        defaultBrightness = DEFAULT_BRIGHTNESS_LOW;
    }

    pixelBuffer.setBrightness(defaultBrightness);
    currentBrightness = defaultBrightness;
    newBrightness = defaultBrightness;

    startBLE();

    // Setup the default pattern to show prior to any BT connections
    newPatternData.colorPattern = ColorPatternType::SingleColor;
    newPatternData.displayPattern = DisplayPatternType::Solid;
    newPatternData.color1 = Adafruit_NeoPixel::Color(230, 22, 161); // Pink
    currentLightStyle = PatternFactory::createForPatternData(newPatternData, &pixelBuffer);
    btService.setPatternData(newPatternData);
}

// Main loop --
// This method is called continously.
void loop()
{
    BLE.poll();
    emitTelemetry();
    checkForLowPowerState();

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
    pinMode(LOW_BRIGHTNESS_PIN, INPUT_PULLUP);
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

    SignConfigurationData configData;
    configData.signType = signType;
    configData.signOrder = signPosition;
    configData.columnCount = pixelBuffer.getColumnCount();
    configData.pixelCount = pixelBuffer.getPixelCount();

    btService.initialize(BTCOMMON_SECONDARYCONTROLLER_UUID, localName);
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

void blinkLowPowerIndicator()
{
    // Turn all LEDs off except for the first one, which will blink red.
    pixelBuffer.setBrightness(255);
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
        if (currentLightStyle)
        {
            delete currentLightStyle;
        }

        currentLightStyle = PatternFactory::createForPatternData(newPatternData, &pixelBuffer);
        currentLightStyle->setSpeed(newSpeed);
        currentLightStyle->reset();

        currentPatternData = newPatternData;
    }

    // The current style shouldn't ever be null here, but check anyways.
    if (currentLightStyle)
    {
        currentLightStyle->update();
    }

    pixelBuffer.displayPixels();
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
    pixelBuffer.setDigitsToLeft(offsetData.digitsToLeft);
    pixelBuffer.setDigitsToRight(offsetData.digitsToRight);
    pixelBuffer.setColsToLeft(offsetData.columnsToLeft);
    pixelBuffer.setColsToRight(offsetData.columnsToRight);
}
