#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoBLE.h>
#include <vector>
#include "PixelBuffer.h"
#include "LightStyles/LightStyle.h"
#include "LightStyles/SingleColorStyle.h"
#include "LightStyles/TwoColorStyle.h"
#include "LightStyles/RainbowStyle.h"
#include "Bluetooth.h"

#ifndef SECONDARYCONTROLLER_MAIN_H
#define SECONDARYCONTROLLER_MAIN_H

// Input-Output pin assignments
#define DATA_OUT 25           // GPIO pin # (NOT Digital pin #) controlling the NeoPixels
#define VOLTAGEINPUTPIN 14    // The pin # for the analog input to detect battery voltage level.

// Initial default values for LED styles
#define DEFAULTSTYLE 0        // The default style to start with. This is an index into the lightStyles vector.
#define DEFAULTBRIGHTNESS 20  // Brightness should be between 0 and 255.
#define DEFAULTSPEED 50       // Speed should be between 1 and 100.
#define DEFAULTSTEP  50       // Step should be between 1 and 100.
#define DEFAULTPATTERN 0      // Default pattern (ie, Row/Column/Digit/etc). This is an index into the LightStyle::knownPatterns vector.

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

#endif