#ifndef SECONDARYCONTROLLER_MAIN_H
#define SECONDARYCONTROLLER_MAIN_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoBLE.h>
#include <vector>
#include <BluetoothCommon.h>
#include "Bluetooth\SecondaryPeripheral.h"
#include <SignDataLib.h>
#include <DisplayPatternLib.h>
#include <PredefinedStyleLib.h>
#include <PushButton.h>
#include <Configuration.h>
#include "NeoPixelDisplay.h"

// Input-Output pin assignments
#define DATA_OUT 25            // GPIO pin # (NOT Digital pin #) controlling the NeoPixels
#define VOLTAGEINPUTPIN 26     // The GPIO pin # for the analog input to detect battery voltage level.

// The order and styletype pinouts need to be updated.
// Style type isn't yet used.
// For the selector pins, the most significant bit should be listed first.
#define ORDER_SELECTOR_PINS  20, 21, 5 // The set of GPIO pin #s that tell the controller what position the sign should be in (MSB to LSB).
#define STYLE_TYPE_SELECTOR_PINS 16, 17, 18, 19  // The set of GPIO pin #s that tell the controller what style (digit # or logo) the sign should be (MSB to LSB).
#define POWER_BUTTON_INPUT_PIN 15   // The GPIO pin # corresponding to the power button. This is a momentary switch that pulls low when pressed.

// Initial default values for LED styles
#define DEFAULT_BRIGHTNESS 170  // Brightness should be between 0 and 255.
#define DEFAULT_SPEED 90        // Speed should be between 1 and 100.
#define LOW_BRIGHTNESS_PIN   12  // When this GPIO pin # is pulled low, the default brightness will be much lower than normal.
#define DEFAULT_BRIGHTNESS_LOW 20 // The default brightness to use when the "Low Brightness" pin has been pulled low.

// Batter power monitoring
#define LOWPOWERTHRESHOLD 6.7     // The voltage below which the system will go into "low power" mode.
#define NORMALPOWERTHRESHOLD 7.2  // The voltage above which the system will recover from "low power" mode.
#define VOLTAGEMULTIPLIER 4.83    // The value to multiply the analog reading by to get the actual voltage.
#define POWER_INDICATOR_PIN 19    // The digital output pin that has the LED to indicate the power status.

// Debugging info
#define INITIALDELAY 500        // Startup delay (in msec) for debugging.
#define TELEMETRYINTERVAL 2000  // The amount of time (in msec) between timing calculations.
#define TIMESTAMPUPDATEINTERVAL 500 // The amount of time (in msec) between updates to the 'timestamp' BT characteristic

#define PITSIGN_TYPE_ID 10  // The "type id" for the pit sign

// Function prototypes
void initializeIO();
void startBLE();
void readBleSettings();
void updateLEDs();
float getCalculatedBatteryVoltage();
int getVoltageInputLevel();
void updateTelemetry();
void updateLedTelemetry();
void checkForLowPowerState();
void indicateBleFailure();
byte getSignType();
byte getSignPosition();
void resetPixelBufferOffsets(SignOffsetData configData);
void enterLowPowerMode();
void exitLowPowerMode();
void readInputButton();
void turnOnPowerLed();
void turnOffPowerLed();
void setupStyleList();

#endif