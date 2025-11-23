#include <Arduino.h>
#include <unity.h>
#include <vector>

// Reference any needed cpp files directly to avoid pulling in
// references that can't be resolved with the "fake" Arduino environment.
#include <Configuration.h>
#include <ButtonProcessor.h>
#include <DebugUtils.h>
#include "../TestHelpers/MockButton.cpp"

int lastCallerId(-1);
String lastActionName("");
std::vector<String> lastArguments;
MockButton* button1;
MockButton* button2;

const char* minimalConfigurationJson();
const char* oneButtonWithActions();
const char* buttonsWithActions();
const char* disabledButton();
const char* fullLegacySignConfigurationJson();

void resetActionParameters() {
    lastCallerId = -1;
    lastActionName = "";
    lastArguments.clear();
}

// Run before each test
void setUp(void) {
    resetActionParameters();
    button1 = new MockButton();
    button2 = new MockButton();
}

// Run after each test
void tearDown(void) {
    // Note: buttons are now owned by SystemConfiguration and will be deleted by it
    // So we just reset the pointers here to avoid dangling references
    button1 = nullptr;
    button2 = nullptr;
}

GenericButton* mockButtonFactory(int gpioPin) {
    if (gpioPin == 1) {
        return button1;
    }

    if (gpioPin == 2) {
        return button2;
    }

    return nullptr;
}

void processAction(int callerId, String actionName, std::vector<String> arguments) {
    lastCallerId = callerId;
    lastActionName = actionName;
    lastArguments = arguments;
}

void emptyJsonDoesNothing() {
    String json = "";
    SystemConfiguration* sc = SystemConfiguration::ParseJson(
        json.c_str(), 
        json.length(), 
        mockButtonFactory);
    
    TEST_ASSERT_NOT_NULL_MESSAGE(sc, "SystemConfiguration was null.");
    std::vector<GenericButton*> buttons = sc->getButtonProcessor().getButtons();
    TEST_ASSERT_EQUAL_MESSAGE(0, buttons.size(), "There should be no buttons.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("displayconfiguration.json", sc->getDisplayConfigurationFile().c_str(), "Display configuration file is not the expected default value.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("bluetoothconfiguration.json", sc->getBluetoothConfigurationFile().c_str(), "Bluetooth configuration file is not the expected default value.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("styleconfiguration.json", sc->getStyleConfigurationFile().c_str(), "Style configuration file is not the expected default value.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1.0f, sc->getClockMultiplier(), "Clock multiplier is not the expected default value.");
    // Additional checks when other configs are in place:
    // BatteryMonitorConfiguration is disabled
    // PowerLedConfiguration is disabled
    // Tm1637Configuration is disabled
    delete sc;
}

void emptyJsonObjectDoesNothing() {
    String json = "{}";
    SystemConfiguration* sc = SystemConfiguration::ParseJson(
        json.c_str(), 
        json.length(), 
        mockButtonFactory);
    
    TEST_ASSERT_NOT_NULL_MESSAGE(sc, "SystemConfiguration was null.");
    std::vector<GenericButton*> buttons = sc->getButtonProcessor().getButtons();
    TEST_ASSERT_EQUAL_MESSAGE(0, buttons.size(), "There should be no buttons.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("displayconfiguration.json", sc->getDisplayConfigurationFile().c_str(), "Display configuration file is not the expected default value.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("bluetoothconfiguration.json", sc->getBluetoothConfigurationFile().c_str(), "Bluetooth configuration file is not the expected default value.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("styleconfiguration.json", sc->getStyleConfigurationFile().c_str(), "Style configuration file is not the expected default value.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1.0f, sc->getClockMultiplier(), "Clock multiplier is not the expected default value.");
    // Additional checks when other configs are in place:
    // BatteryMonitorConfiguration is disabled
    // PowerLedConfiguration is disabled
    // Tm1637Configuration is disabled
    delete sc;
}

void minimalJsonSetsProperties() {
    String json = minimalConfigurationJson();
    SystemConfiguration* sc = SystemConfiguration::ParseJson(
        json.c_str(), 
        json.length(), 
        mockButtonFactory);
    
    std::vector<GenericButton*> buttons = sc->getButtonProcessor().getButtons();
    TEST_ASSERT_EQUAL_MESSAGE(0, buttons.size(), "There should be no buttons.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("display.json", sc->getDisplayConfigurationFile().c_str(), "Display configuration file is not the expected value.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("bt.json", sc->getBluetoothConfigurationFile().c_str(), "Bluetooth configuration file is not the expected value.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("styles.json", sc->getStyleConfigurationFile().c_str(), "Style configuration file is not the expected value.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1.0f, sc->getClockMultiplier(), "Clock multiplier is not the expected default value.");
    // Additional checks when other configs are in place:
    // BatteryMonitorConfiguration is disabled
    // PowerLedConfiguration is disabled
    // Tm1637Configuration is disabled
    delete sc;
}

void buttonsAndActionsAreParsed() {
    String json = buttonsWithActions();
    SystemConfiguration* sc = SystemConfiguration::ParseJson(
        json.c_str(), 
        json.length(), 
        mockButtonFactory);
    
    ButtonProcessor& bp = sc->getButtonProcessor();
    std::vector<GenericButton*> buttons = bp.getButtons();
    TEST_ASSERT_EQUAL_MESSAGE(2, buttons.size(), "There should be 2 buttons.");
    bp.setActionProcessor(processAction);

    // Simulate button 1 tap
    button1->setPressType(ButtonPressType::Normal);
    sc->getButtonProcessor().update();
    TEST_ASSERT_EQUAL_STRING_MESSAGE("changeStyle", lastActionName.c_str(), "Last action name is not correct for button 1 tap.");
    TEST_ASSERT_EQUAL_MESSAGE(2, lastArguments.size(), "Number of arguments is not correct for button 1 tap.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Pink", lastArguments[0].c_str(), "First argument is not correct for button 1 tap.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("RainbowLava", lastArguments[1].c_str(), "Second argument is not correct for button 1 tap.");
    resetActionParameters();

    // Simulate button 1 long tap
    button1->setPressType(ButtonPressType::Long);
    sc->getButtonProcessor().update();
    TEST_ASSERT_EQUAL_STRING_MESSAGE("batteryVoltage", lastActionName.c_str(), "Last action name is not correct for button 1 long tap.");
    TEST_ASSERT_EQUAL_MESSAGE(0, lastArguments.size(), "Number of arguments is not correct for button 1 long tap.");
    resetActionParameters();

    // Simulate button 2 tap
    button2->setPressType(ButtonPressType::Normal);
    sc->getButtonProcessor().update();
    TEST_ASSERT_EQUAL_STRING_MESSAGE("changeStyle", lastActionName.c_str(), "Last action name is not correct for button 2 tap.");
    TEST_ASSERT_EQUAL_MESSAGE(3, lastArguments.size(), "Number of arguments is not correct for button 2 tap.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Red", lastArguments[0].c_str(), "First argument is not correct for button 2 tap.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Blue", lastArguments[1].c_str(), "Second argument is not correct for button 2 tap.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Pink", lastArguments[2].c_str(), "Third argument is not correct for button 2 tap.");
    resetActionParameters();

    delete sc;
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(emptyJsonDoesNothing);
    RUN_TEST(emptyJsonObjectDoesNothing);
    RUN_TEST(minimalJsonSetsProperties);
    RUN_TEST(buttonsAndActionsAreParsed);

    return UNITY_END();
}

const char* minimalConfigurationJson() {
    return R"json(
        {
            "displayConfigurationFile": "display.json",
            "bluetoothConfigurationFile": "bt.json",
            "styleConfigurationFile": "styles.json"
        }
    )json";
}

const char* buttonsWithActions() {
    return R"json(
        {
            "buttons": {
                "definitions": [
                    {
                        "id": "1",
                        "enabled": true,
                        "gpioPin": 1
                    },
                    {
                        "id": "2",
                        "enabled": true,
                        "gpioPin": 2
                    }
                ],
                "actions": [
                    {
                        "buttonIds": ["1"],
                        "tapAction": "changeStyle",
                        "longTapAction": "batteryVoltage",
                        "tapActionArguments": ["Pink", "RainbowLava"],
                        "longTapActionArguments": []
                    },
                    {
                        "buttonIds": ["2"],
                        "tapAction": "changeStyle",
                        "tapActionArguments": ["Red", "Blue", "Pink"]
                    },
                    {
                        "buttonIds": ["1", "2"],
                        "tapAction": "changeStyle",
                        "tapActionArguments": ["Fire"]
                    }
                ]
            }
        }
    )json";
}

const char* disabledButton() {
    return R"json(
        {
            "buttons": {
                "definitions": [
                    {
                        "id": "1",
                        "enabled": true,
                        "gpioPin": 1
                    },
                    {
                        "id": "2",
                        "enabled": false,
                        "gpioPin": 2
                    }
                ],
                "actions": [
                    {
                        "buttonIds": ["1"],
                        "tapAction": "changeStyle",
                        "tapActionArguments": ["Pink", "RainbowLava"]
                    },
                    {
                        "buttonId": ["2"],
                        "tapAction": "changeStyle",
                        "tapActionArguments": ["Fire"]
                    }
                ]
            }
        }
    )json";
}

const char* fullLegacySignConfigurationJson()
{
    return R"json(
        {
            "comment": "Legacy sign",
            "displayConfigurationFile": "displayconfiguration.json",
            "bluetoothConfigurationFile": "bluetoothconfiguration.json",
            "styleConfigurationFile": "styleconfigurations.json"
            "buttons": {
                "definitions": [
                    {
                        "id": "1",
                        "enabled": true,
                        "gpioPin": 27
                    },
                    {
                        "id": "2",
                        "enabled": true,
                        "gpioPin": 28
                    },
                    {
                        "id": "3",
                        "enabled": true,
                        "gpioPin": 26
                    }
                ],
                "actions": [
                    {
                        "buttonIds": ["1"],
                        "tapAction": "changeStyle",
                        "tapActionArguments": ["Pink", "RainbowRandom"],
                        "longTapAction": "changeStyle",
                        "longTapActionArguments": ["Fire"]
                    },
                    {
                        "buttonId": ["2"],
                        "tapAction": "changeStyle",
                        "tapActionArguments": ["BluePinkRandom", "BluePinkDigit"]
                        "longTapAction": "batteryVoltage",
                        "longTapActionArguments": []
                    },
                    {
                        "buttonId": ["3"],
                        "tapAction": "changeStyle",
                        "tapActionArguments": ["RedPinkRandom", "RedPinkDigit"]
                        "longTapAction": "disconnectBT",
                        "longTapActionArguments": []
                    }
                ]
            },
            "batteryMonitor": {
                "enabled": true,
                "analogInputGpioPin": 29,
                "inputMultiplier": 4.83,
                "voltageToEnterLowPowerState": 6.7,
                "voltageToExitLowPowerState": 7.2
            },
            "powerLed": {
                "enabled": false,
                "gpioPin": 22
            },
            "clockMultiplier": 1.0,
            "tm1637Display": {
                "enabled": true,
                "clockGpioPin": 15,
                "dataGpioPin": 16,
                "brightness": 5
            }
        }
    )json";
}