#ifndef SECONDARYCONTROLLER_MAIN_H
#define SECONDARYCONTROLLER_MAIN_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoBLE.h>
#include <vector>
#include "PixelBuffer.h"
#include "LightStyles/LightStyle.h"
#include "LightStyles/SingleColorStyle.h"
#include "LightStyles/TwoColorStyle.h"
#include "LightStyles/RainbowStyle.h"
#include <BluetoothCommon.h>
#include "Bluetooth\SecondaryPeripheral.h"

// Input-Output pin assignments
#define DATA_OUT 25           // GPIO pin # (NOT Digital pin #) controlling the NeoPixels
#define VOLTAGEINPUTPIN 17    // The pin # (Digital #) for the analog input to detect battery voltage level.

// The order and styletype pinouts need to be updated.
// Style type isn't yet used.
// For the selector pins, the most significant bit should be listed first.
#define ORDER_SELECTOR_PINS  8, 9, 10 // The set of Digital pin #s that tell the controller what position the sign should be in.
#define STYLE_TYPE_SELECTOR_PINS 4, 5, 6, 7 // The set of Digital pin #s that tell the controller what style (digit # or logo) the sign should is.

// Initial default values for LED styles
#define DEFAULTSTYLE 0        // The default style to start with. This is an index into the lightStyles vector.
#define DEFAULTBRIGHTNESS 20  // Brightness should be between 0 and 255.
#define DEFAULTSPEED 90       // Speed should be between 1 and 100.
#define DEFAULTSTEP  70       // Step should be between 1 and 100.
#define DEFAULTPATTERN 6      // Default pattern (ie, Row/Column/Digit/etc). This is an index into the LightStyle::knownPatterns vector.

// Batter power monitoring
#define LOWPOWERTHRESHOLD 5.9     // The voltage below which the system will go into "low power" mode.
#define NORMALPOWERTHRESHOLD 6.9  // The voltage above which the system will recover from "low power" mode.

// Debugging info
#define INITIALDELAY 500        // Startup delay (in msec) for debugging.
#define TELEMETRYINTERVAL 2000  // The amount of time (in msec) between timing calculations.

// Function prototypes
void initializeIO();
void initializeLightStyles();
void startBLE();
void blinkLowPowerIndicator();
void readBleSettings();
void updateBrightness();
void updateLEDs();
byte isInRange(byte value, byte minValue, byte maxValue);
float getCalculatedBatteryVoltage();
int getVoltageInputLevel();
void emitTelemetry();
void indicateBleFailure();
byte getSignType();
byte getSignPosition();
void initializePixelBuffer();

#endif