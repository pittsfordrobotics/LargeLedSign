#include "main.h"

// Global variables
CommonPeripheral btService;
StatusDisplay display(TM1637_CLOCK, TM1637_DIO, TM1637_BRIGHTNESS);
NeoPixelDisplay* neoPixelDisplay;
DisplayPattern* currentLightStyle;
ButtonProcessor buttonProcessor;
StyleConfiguration* styleConfiguration;

ulong loopCounter = 0;
ulong lastTelemetryTimestamp = 0;
ulong ledLoopCounter = 0;
ulong lastLedTelemetryTimestamp = 0;

int lastManualButtonPressed = -1;
int manualButtonSequenceNumber = 0;
byte inLowPowerMode = false;          // Indicates the system should be in "low power" mode. This should be a boolean, but there are no bool types.

// Settings that are updated via bluetooth
byte currentBrightness = DEFAULT_BRIGHTNESS;
byte newBrightness = DEFAULT_BRIGHTNESS;
byte currentSpeed;
byte newSpeed;
PatternData currentPatternData;
PatternData newPatternData;

volatile bool isInitialized = false;

void setup()
{
    Serial.begin(9600);
    delay(2000);
    Serial.println("Starting...");

    initializeIO();
    initializeButtonProcessor();
    int signType = LEGACY_SIGN_TYPE;
    if (digitalRead(LOW_BRIGHTNESS_PIN) == LOW)
    {
        currentBrightness = DEFAULT_BRIGHTNESS_LOW;
        newBrightness = DEFAULT_BRIGHTNESS_LOW;
        signType = 0; // Test matrix
    }

    display.setDisplay("---1");
    display.setDisplay("---2");

    //
    // Read from config!!!
    //
    std::vector<DisplayConfiguration>* displayConfigs;
    if (signType == 0)
    {
        displayConfigs = DisplayConfigFactory::createForTestMatrix();
    }
    else
    {
        displayConfigs = DisplayConfigFactory::createForLegacySign();
    }

    styleConfiguration = StyleConfigFactory::createDefaultStyleConfiguration();

    neoPixelDisplay = new NeoPixelDisplay(displayConfigs->at(0));
    neoPixelDisplay->setBrightness(currentBrightness);
    newPatternData = styleConfiguration->getDefaultStyle().getPatternData();
    currentPatternData = newPatternData;
    DisplayPattern* initialPattern = PatternFactory::createForPatternData(newPatternData);
    currentSpeed = styleConfiguration->getDefaultStyle().getSpeed();
    newSpeed = currentSpeed;
    initialPattern->setSpeed(newSpeed);
    neoPixelDisplay->setDisplayPattern(initialPattern);

    display.setDisplay("---3");

    if (!BLE.begin())
    {
        Serial.println("BLE initialization failed!");
        display.setDisplay("E-1");
        while (true)
        {
            Serial.println("BLE initialization failed!");
            delay(1000);
        }
    }

    startBLEService();
    isInitialized = true;
}

void loop()
{
    BLE.poll();
    display.update();
    updateTelemetry();
    checkForLowPowerState();

    if (btService.isConnected())
    {
        // Something is connected via BT.
        // Set the display to "--" to show something connected to us.
        // Display it as "temporary" since it's a low-priority message.
        display.displayTemporary(" --", 200);
    }

    if (!inLowPowerMode)
    {
        // Only process inputs if we're not in low power mode.
        readSettingsFromBLE();
        buttonProcessor.update();
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
    updateLedTelemetry();
}

void initializeButtonProcessor()
{
    buttonProcessor.setActionProcessor(ProcessButtonAction);

    // Add the buttons
    std::vector<int> manualInputPins{MANUAL_INPUT_PINS};
    for (uint i = 0; i < manualInputPins.size(); i++)
    {
        buttonProcessor.addButtonDefinition(String(i), new ArduinoPushButton(manualInputPins[i], INPUT_PULLUP));
    }

    // Add the button actions
    buttonProcessor.addTapAction({"0"}, "changeStyle", {"RainbowRandom", "Pink"});
    buttonProcessor.addTapAction({"1"}, "changeStyle", {"BluePinkRandom", "BluePinkDigit"});
    buttonProcessor.addTapAction({"2"}, "changeStyle", {"RedPinkRandom", "RedPinkDigit"});
    buttonProcessor.addLongTapAction({"0"}, "changeStyle", {"Fire"});
    buttonProcessor.addLongTapAction({"1"}, "batteryVoltage");
    buttonProcessor.addLongTapAction({"2"}, "disconnectBT");
}

void setManualStyle(StyleDefinition styleDefinition)
{
    newSpeed = styleDefinition.getSpeed();
    newPatternData = styleDefinition.getPatternData();

    // Update the local BLE settings to reflect the new manual settings.
    btService.setSpeed(newSpeed);
    btService.setPatternData(newPatternData);
}

void startBLEService()
{
    display.setDisplay("C  =");
    Serial.println("Setting up Peripheral service using common logic.");
    String localName = "Small 3181 Sign";
    btService.initialize(BTCOMMON_PRIMARYCONTROLLER_UUID, localName);

    // Set the various characteristics based on the defaults
    display.setDisplay("C ==");

    btService.setBrightness(currentBrightness);
    btService.setSpeed(styleConfiguration->getDefaultStyle().getSpeed());
    btService.setPatternData(styleConfiguration->getDefaultStyle().getPatternData());
    btService.setColorPatternList(PatternFactory::getKnownColorPatterns());
    btService.setDisplayPatternList(PatternFactory::getKnownDisplayPatterns());

    Serial.println("Peripheral service started.");
    display.clear();
}

void readSettingsFromBLE()
{
    newBrightness = btService.getBrightness();
    newSpeed = btService.getSpeed();
    newPatternData = btService.getPatternData();
}

void initializeIO()
{
    pinMode(VOLTAGEINPUTPIN, INPUT);
    pinMode(LOW_BRIGHTNESS_PIN, INPUT_PULLUP);
}

// Read the analog input from the "voltage input" pin and calculate the battery voltage.
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

void displayBatteryVoltage()
{
    double voltage = getCalculatedBatteryVoltage();
    display.displayTemporary(String(voltage, 2), 1500);
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
            neoPixelDisplay->setBrightness(currentBrightness);
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

void updateLEDs()
{
    if (newBrightness != currentBrightness)
    {
        neoPixelDisplay->setBrightness(newBrightness);
        currentBrightness = newBrightness;
    }

    if (newSpeed != currentSpeed)
    {
        if (currentLightStyle)
        {
            currentLightStyle->setSpeed(newSpeed);
        }
        
        currentSpeed = newSpeed;
    }

    if (currentPatternData != newPatternData)
    {
        DisplayPattern* oldPattern = neoPixelDisplay->getDisplayPattern();;
        DisplayPattern* newPattern = PatternFactory::createForPatternData(newPatternData);
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

void ProcessButtonAction(int callerId, String actionName, std::vector<String> arguments)
{
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
