#include <Arduino.h>
#include <ArduinoBLE.h>
#include <vector>
#include <algorithm>
#include <BluetoothCommon.h>
#include <DisplayPatternLib.h>
#include <PushButton.h>
#include <StatusDisplay.h>
#include <PredefinedStyleLib.h>
#include <PixelBufferLib.h>

#define DATA_OUT 25            // GPIO pin # (NOT Digital pin #) controlling the NeoPixels
#define VOLTAGEINPUTPIN 29     // The pin # (Digital #) for the analog input to detect battery voltage level.

#define TM1637_CLOCK 7    // Digital pin # for the TM1637 clock line
#define TM1637_DIO 5      // Digital pin # for the TM1637 data line
#define TM1637_BRIGHTNESS 5  // Brightness of the TM1637, between 0 and 7

#define LOWPOWERTHRESHOLD 6.7     // The voltage below which the system will go into "low power" mode.
#define NORMALPOWERTHRESHOLD 7.2  // The voltage above which the system will recover from "low power" mode.
#define VOLTAGEMULTIPLIER 4.83    // The value to multiply the analog reading by to get the actual voltage.

#define MANUAL_INPUT_PINS 27, 28, 26      // Digital pin #s for the manual input buttons.

#define TELEMETRYINTERVAL 2000     // Interval (msec) for updating the telemetry.

#define DEFAULT_BRIGHTNESS 255  // Brightness should be between 0 and 255.
#define LOW_BRIGHTNESS_PIN  4  // When this Digital pin # is pulled low, the default brightness will be much lower than normal.
#define DEFAULT_BRIGHTNESS_LOW 20 // The default brightness to use when the "Low Brightness" pin has been pulled low.

#define LEGACY_SIGN_TYPE 16     // The "sign type" corresponding to the Legacy sign, used to initialize the pixel buffer.

// Function prototypes
void initializeIO();
void setupStyleLists();
void startBLEService();
void readSettingsFromBLE();
void processManualInputs();
void updateInputButtons();
void setManualStyle(PredefinedStyle style);
void updateTelemetry();
float getCalculatedBatteryVoltage();
int getVoltageInputLevel();
void displayBatteryVoltage();
void checkForLowPowerState();
void updateLEDs();