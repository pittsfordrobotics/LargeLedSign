#include "main.h"

// Input pins should be 3 for position, 4 for style (ie, digits 0-9 and logo)
// Move the numbers to a #define.
// Should use D4 - D10, but final pinout is still TBD.
// Alternative: make this a vector instead so the loops can utilize .size().
// Ex:
// std::vector<int> orderSelectorPins = { ORDER_SELECTOR_PINS };
// std::vector<int> signTypeSelectorPins = { SIGN_TYPE_SELECTOR_PINS };
int orderSelectorPins[] = {7, 8}; // Tells the controller which digit it's controlling (only the first pin is used so far)

// Main BLE service wrapper
Bluetooth btService;

// Pixel and color data
PixelBuffer pixelBuffer(DATA_OUT);
std::vector<LightStyle*> lightStyles;

// Settings that are updated via bluetooth
byte currentBrightness = DEFAULTBRIGHTNESS;
byte newBrightness = DEFAULTBRIGHTNESS;
byte currentStyle = -1; // Force the style to "change" on the first iteration.
byte newStyle = DEFAULTSTYLE;
byte currentSpeed = DEFAULTSPEED;
byte newSpeed = DEFAULTSPEED;
byte currentStep = DEFAULTSTEP;
byte newStep = DEFAULTSTEP;
byte currentPattern = DEFAULTPATTERN;
byte newPattern = DEFAULTPATTERN;

// Other internal state
int loopCounter = 0;              // Records the number of times the main loop ran since the last timing calculation.
unsigned long lastTelemetryTimestamp = 0;  // The last time debug information was emitted.
byte inLowPowerMode = false;      // Indicates the system should be in "low power" mode. This should be a boolean, but there are no bool types.

// Main entry point for the program --
// This is run once at startup.
void setup() {
  // Delay for debugging
  delay(INITIALDELAY);
  Serial.begin(9600);
  Serial.println("Starting...");
  lastTelemetryTimestamp = millis();

  // Initialize components
  pixelBuffer.initialize();
  pixelBuffer.setBrightness(DEFAULTBRIGHTNESS);
  initializeIO();
  initializeLightStyles();
  startBLE();
}

// Main loop --
// This metod is called continously.
void loop()
{  
  //emitTelemetry();
  //checkForLowPowerState();

  if (inLowPowerMode) {
    // blink LEDs and exit.
    blinkLowPowerIndicator();
    return;    
  }

  // See if any settings have been changed via BLE and apply them if necessary.
  readBleSettings();

  // Apply any updates that were received via BLE or manually
  updateBrightness();
  updateLEDs();
}

// Initialize all input/output pins
void initializeIO() {
  Serial.println("Initializing I/O pins.");
  for (int i = 0; i < 2; i++) {
    pinMode(orderSelectorPins[i], INPUT_PULLUP);
  }

  Serial.println("Initializing the analog input to monitor battery voltage.");
  pinMode(VOLTAGEINPUTPIN, INPUT);
}

// Set up the list of "known" light styles.
// These are the styles presented in the "Styles" list in the phone app.
void initializeLightStyles() {
  Serial.println("Initializing light styles");
  lightStyles.push_back(new RainbowStyle("Rainbow", &pixelBuffer));
  uint32_t pink =  Adafruit_NeoPixel::Color(230, 22, 161);
  uint32_t red = Adafruit_NeoPixel::Color(255, 0, 0);
  uint32_t blue = Adafruit_NeoPixel::Color(0, 0, 255);
  //uint32_t white = Adafruit_NeoPixel::Color(255, 255, 255);
  uint32_t orange = Adafruit_NeoPixel::Color(255, 50, 0);
  lightStyles.push_back(new SingleColorStyle("Pink", pink, &pixelBuffer));
  lightStyles.push_back(new TwoColorStyle("Blue-Pink", blue, pink, &pixelBuffer));
  lightStyles.push_back(new SingleColorStyle("Blue", blue, &pixelBuffer));
  lightStyles.push_back(new TwoColorStyle("Red-Pink", red, pink, &pixelBuffer));
  lightStyles.push_back(new SingleColorStyle("Red", red, &pixelBuffer));
  lightStyles.push_back(new TwoColorStyle("Orange-Pink", orange, pink, &pixelBuffer));
  //lightStyles.push_back(new SingleColorStyle("White", white, &pixelBuffer));
}

// Set the initial BLE characteristic values and start the BLE service.
void startBLE() {
  int signType = 0;
  if (digitalRead(orderSelectorPins[0]) == HIGH) {
    signType = 1;
  }

  Serial.print("Initializing for sign type ");
  Serial.println(signType);

  // Use a common uuid
  String uuid = "1221ca8d-4172-4946-bcd1-f9e4b40ba6b0";
  String localName;
  // Longer term - read sign position and digit from inputs,
  // set local name to "3181 LED Controller <position>-<digit>"
  // Digits can be 0-9, with the gear logo being 10.
  switch (signType) {
    case 0:
      localName = "3181 LED Controller 1-3";
      break;
    case 1:
      localName = "3181 LED Controller 2-1";
      break;
    default:
      Serial.println("Unrecognized sign type. BLE not configured.");
      return;
  }

  btService.initialize(uuid, localName);

  std::vector<String> styleNames;
  for (uint i = 0; i < lightStyles.size(); i++) {
    styleNames.push_back(lightStyles[i]->getName());    
  }  

  btService.setStyleNames(styleNames);
  btService.setPatternNames(LightStyle::knownPatterns);
  btService.setBrightness(DEFAULTBRIGHTNESS);
  btService.setStyle(DEFAULTSTYLE);
  btService.setSpeed(DEFAULTSPEED);
  btService.setPattern(DEFAULTPATTERN);
  btService.setStep(DEFAULTSTEP);
}

// Read the BLE settings to see if any have been changed.
void readBleSettings() {
  newBrightness = btService.getBrightness();

  // Check the range on the characteristic values.
  // If out of range, ignore the update and reset the BLE characteristic to the old value.
  newStyle = btService.getStyle();
  if (!isInRange(newStyle, 0, lightStyles.size()-1)) {
    btService.setStyle(currentStyle);
    newStyle = currentStyle;
  }

  newSpeed = btService.getSpeed();
  if (!isInRange(newSpeed, 1, 100)) {
    btService.setSpeed(currentSpeed);
    newSpeed = currentSpeed;
  }

  newStep = btService.getStep();
  if (!isInRange(newStep, 1, 100)) {
    btService.setStep(currentStep);
    newStep = currentStep;
  }
  
  newPattern = btService.getPattern();
  if (!isInRange(newPattern, 0, LightStyle::knownPatterns.size()-1)) {
    btService.setPattern(currentPattern);
    newPattern = currentPattern;
  }
}

// Determine if the give byte value is between (or equal to) the min and max values.
byte isInRange(byte value, byte minValue, byte maxValue) {
  return (value >= minValue && value <= maxValue);
}

void blinkLowPowerIndicator() {
  // Turn all LEDs off except for the first one, which will blink red.
  pixelBuffer.clearBuffer();
  pixelBuffer.displayPixels();
  delay(500);

  pixelBuffer.setPixel(0, Adafruit_NeoPixel::Color(255, 0, 0));
  pixelBuffer.displayPixels();
  delay(500);
}

// Set the LEDs to a new brightness if the brightness has changed.
void updateBrightness() {
  if (currentBrightness != newBrightness) {
    Serial.println("Brightness change detected.");
    pixelBuffer.setBrightness(newBrightness);
  }

  currentBrightness = newBrightness;
}

// Set the style properties (speed, step, pattern, etc) if any have changed,
// and call the current style class to update the display.
void updateLEDs() {
  int shouldResetStyle = false;
  LightStyle *style = lightStyles[newStyle];
  if (currentStyle != newStyle)  
  {
    Serial.print("Changing style to ");
    Serial.println(newStyle);
    // Changing styles - reset the lights
    style->setSpeed(currentSpeed);
    style->setStep(currentStep);
    style->setPattern(currentPattern);
    shouldResetStyle = true;
    currentStyle = newStyle;
  }

  if (currentSpeed != newSpeed) {
    style->setSpeed(newSpeed);
    currentSpeed = newSpeed;
  }

  if (currentStep != newStep) {
    style->setStep(newStep);
    currentStep = newStep;
  }

  if (currentPattern != newPattern) {
    style->setPattern(newPattern);
    shouldResetStyle = true;
    currentPattern = newPattern;
  }

  if (shouldResetStyle) {
    style->reset();
  }

  style->update();
  pixelBuffer.displayPixels();
}

// Check if the current battery voltage is too low to run the sign,
// or if the battery has been charged enough to restart operation.
void checkForLowPowerState()
{
  double currentVoltage = getCalculatedBatteryVoltage();

  // Check if the voltage is too low.
  if (currentVoltage < LOWPOWERTHRESHOLD) {
    if (inLowPowerMode) {
      // Already in low power mode - nothing to do.
    } else {
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
      //btService.stop();
      inLowPowerMode = true;
    }
  }

  // Check if the voltage is high enough for normal operation.
  if (currentVoltage > NORMALPOWERTHRESHOLD) {
    if (!inLowPowerMode) {
      // Not in low power mode - nothing to do.
    } else {
      Serial.print("Battery voltage is above normal threshold. Voltage: ");
      Serial.print(currentVoltage);
      Serial.print(", threshold: ");
      Serial.println(NORMALPOWERTHRESHOLD);
      Serial.println("Exiting low power mode.");
      // Exit low power mode.  Re-enable BLE.
      //btService.resume();
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
