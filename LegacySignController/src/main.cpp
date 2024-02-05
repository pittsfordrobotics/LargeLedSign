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
PixelBuffer pixelBuffer(DATA_OUT);
DisplayPattern* currentLightStyle;

std::vector<PushButton *> manualInputButtons;
PredefinedStyleList* predefinedStyleList;
ulong loopCounter = 0;
ulong lastTelemetryTimestamp = 0;

int lastManualButtonPressed = -1;
int manualButtonSequenceNumber = 0;
byte inLowPowerMode = false;          // Indicates the system should be in "low power" mode. This should be a boolean, but there are no bool types.

PredefinedStyle defaultStyle = PredefinedStyle::getPredefinedStyle(PredefinedStyles::Pink_Solid);
PredefinedStyle lowPowerStyle = PredefinedStyle::getPredefinedStyle(PredefinedStyles::LowPower);

// Settings that are updated via bluetooth
byte currentBrightness = DEFAULT_BRIGHTNESS;
byte newBrightness = DEFAULT_BRIGHTNESS;
byte currentSpeed;
byte newSpeed;
PatternData currentPatternData;
PatternData newPatternData;

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

    initializeIO();

    if (digitalRead(LOW_BRIGHTNESS_PIN) == LOW)
    {
        currentBrightness = DEFAULT_BRIGHTNESS_LOW;
        newBrightness = DEFAULT_BRIGHTNESS_LOW;
    }

    display.setDisplay("----");
    setupStyleLists();
    pixelBuffer.initialize(LEGACY_SIGN_TYPE);
    pixelBuffer.setBrightness(currentBrightness);

    if (!BLE.begin())
    {
        Serial.println("BLE initialization failed!");
        display.setDisplay("E1");
        while (true)
        {
        }
    }

    startBLEService();
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
        updateInputButtons();
        processManualInputs();
    }

    updateLEDs();
}

void processManualInputs()
{
    // First look for any long-presses that happened.
    // If button 1 (id 0) was long-pressed, display battery voltages for the clients.
    if (manualInputButtons[0]->wasPressed() && manualInputButtons[0]->lastPressType() == ButtonPressType::Long)
    {
        manualInputButtons[0]->clearPress();
        displayBatteryVoltage();
    }

    // If button 3 (id 2) was long-pressed, force-disconnect any BT clients.
    if (manualInputButtons[2]->wasPressed() && manualInputButtons[2]->lastPressType() == ButtonPressType::Long)
    {
        manualInputButtons[2]->clearPress();
        btService.disconnect();
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
            PredefinedStyle selectedStyle = predefinedStyleList->getStyle(i, manualButtonSequenceNumber);
            setManualStyle(selectedStyle);
            manualInputButtons[i]->clearPress();
            lastManualButtonPressed = i;
        }
    }
}

void setManualStyle(PredefinedStyle style)
{
    newSpeed = style.getSpeed();
    newPatternData = style.getPatternData();

    // Update the local BLE settings to reflect the new manual settings.
    btService.setSpeed(newSpeed);
    btService.setPatternData(newPatternData);
}

void updateInputButtons()
{
    for (uint i = 0; i < manualInputButtons.size(); i++)
    {
        manualInputButtons[i]->update();
    }
}

void startBLEService()
{
    display.setDisplay("C  =");
    Serial.println("Setting up Peripheral service using common logic.");
    String localName = "3181 LED Legacy Controller";
    btService.initialize(BTCOMMON_PRIMARYCONTROLLER_UUID, localName);

    // Set the various characteristics based on the defaults
    display.setDisplay("C ==");

    btService.setBrightness(currentBrightness);
    btService.setSpeed(defaultStyle.getSpeed());
    btService.setPatternData(defaultStyle.getPatternData());
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

void setupStyleLists()
{
    predefinedStyleList = new PredefinedStyleList(manualInputButtons.size());

    // Styles for button 1 (id 0)
    predefinedStyleList->addStyleToList(0, PredefinedStyles::Pink_Solid);
    predefinedStyleList->addStyleToList(0, PredefinedStyles::Rainbow_Right);

    // Styles for button 2 (id 1)
    predefinedStyleList->addStyleToList(1, PredefinedStyles::RedPink_Right);
    predefinedStyleList->addStyleToList(1, PredefinedStyles::RedPink_CenterOut);

    // Styles for button 3 (id 2)
    predefinedStyleList->addStyleToList(2, PredefinedStyles::BluePink_Right);
    predefinedStyleList->addStyleToList(2, PredefinedStyles::BluePink_CenterOut);
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
            newBrightness = 255;
            newPatternData = lowPowerStyle.getPatternData();
            newSpeed = lowPowerStyle.getSpeed();
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

void updateLEDs()
{
    if (newBrightness != currentBrightness)
    {
        pixelBuffer.setBrightness(newBrightness);
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
