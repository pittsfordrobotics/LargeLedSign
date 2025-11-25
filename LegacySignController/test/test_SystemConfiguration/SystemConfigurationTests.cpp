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
const char* additionalConfigurations();

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
    TEST_ASSERT_FALSE_MESSAGE(sc->isValid(), "SystemConfiguration should be invalid.");
    std::vector<GenericButton*> buttons = sc->getButtonProcessor().getButtons();
    TEST_ASSERT_EQUAL_MESSAGE(0, buttons.size(), "There should be no buttons.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("displayconfiguration.json", sc->getDisplayConfigurationFile().c_str(), "Display configuration file is not the expected default value.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("styleconfiguration.json", sc->getStyleConfigurationFile().c_str(), "Style configuration file is not the expected default value.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1.0f, sc->getClockMultiplier(), "Clock multiplier is not the expected default value.");
    BatteryMonitorConfiguration& bmc = sc->getBatteryMonitorConfiguration();
    TEST_ASSERT_FALSE_MESSAGE(bmc.isEnabled(), "BatteryMonitorConfiguration should be disabled by default.");
    PowerLedConfiguration& plc = sc->getPowerLedConfiguration();
    TEST_ASSERT_FALSE_MESSAGE(plc.isEnabled(), "PowerLedConfiguration should be disabled by default.");
    Tm1637DisplayConfiguration& tdc = sc->getTm1637DisplayConfiguration();
    TEST_ASSERT_FALSE_MESSAGE(tdc.isEnabled(), "Tm1637DisplayConfiguration should be disabled by default.");
    BluetoothConfiguration& btc = sc->getBluetoothConfiguration();
    TEST_ASSERT_TRUE_MESSAGE(btc.isEnabled(), "BluetoothConfiguration should be enabled by default.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("99be4fac-c708-41e5-a149-74047f554cc1", btc.getUuid().c_str(), "Bluetooth UUID is not the expected default value.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("LED Sign Controller", btc.getLocalName().c_str(), "Bluetooth local name is not the expected default value.");
    delete sc;
}

void inValidJsonIsInvalid() {
    String json = "{\"foo\":}";
    SystemConfiguration* sc = SystemConfiguration::ParseJson(
        json.c_str(), 
        json.length(), 
        mockButtonFactory);
    
    TEST_ASSERT_NOT_NULL_MESSAGE(sc, "SystemConfiguration was null.");
    TEST_ASSERT_FALSE_MESSAGE(sc->isValid(), "SystemConfiguration should be invalid.");
    delete sc;
}

void emptyJsonObjectDoesNothing() {
    String json = "{}";
    SystemConfiguration* sc = SystemConfiguration::ParseJson(
        json.c_str(), 
        json.length(), 
        mockButtonFactory);
    
    TEST_ASSERT_NOT_NULL_MESSAGE(sc, "SystemConfiguration was null.");
    TEST_ASSERT_TRUE_MESSAGE(sc->isValid(), "SystemConfiguration should be valid.");
    std::vector<GenericButton*> buttons = sc->getButtonProcessor().getButtons();
    TEST_ASSERT_EQUAL_MESSAGE(0, buttons.size(), "There should be no buttons.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("displayconfiguration.json", sc->getDisplayConfigurationFile().c_str(), "Display configuration file is not the expected default value.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("styleconfiguration.json", sc->getStyleConfigurationFile().c_str(), "Style configuration file is not the expected default value.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1.0f, sc->getClockMultiplier(), "Clock multiplier is not the expected default value.");
    BatteryMonitorConfiguration& bmc = sc->getBatteryMonitorConfiguration();
    TEST_ASSERT_FALSE_MESSAGE(bmc.isEnabled(), "BatteryMonitorConfiguration should be disabled by default.");
    PowerLedConfiguration& plc = sc->getPowerLedConfiguration();
    TEST_ASSERT_FALSE_MESSAGE(plc.isEnabled(), "PowerLedConfiguration should be disabled by default.");
    Tm1637DisplayConfiguration& tdc = sc->getTm1637DisplayConfiguration();
    TEST_ASSERT_FALSE_MESSAGE(tdc.isEnabled(), "Tm1637DisplayConfiguration should be disabled by default.");
    BluetoothConfiguration& btc = sc->getBluetoothConfiguration();
    TEST_ASSERT_TRUE_MESSAGE(btc.isEnabled(), "BluetoothConfiguration should be enabled by default.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("99be4fac-c708-41e5-a149-74047f554cc1", btc.getUuid().c_str(), "Bluetooth UUID is not the expected default value.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("LED Sign Controller", btc.getLocalName().c_str(), "Bluetooth local name is not the expected default value.");
    delete sc;
}

void minimalJsonSetsProperties() {
    String json = minimalConfigurationJson();
    SystemConfiguration* sc = SystemConfiguration::ParseJson(
        json.c_str(), 
        json.length(), 
        mockButtonFactory);
    
    TEST_ASSERT_TRUE_MESSAGE(sc->isValid(), "SystemConfiguration should be valid.");
    std::vector<GenericButton*> buttons = sc->getButtonProcessor().getButtons();
    TEST_ASSERT_EQUAL_MESSAGE(0, buttons.size(), "There should be no buttons.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("display.json", sc->getDisplayConfigurationFile().c_str(), "Display configuration file is not the expected value.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("styles.json", sc->getStyleConfigurationFile().c_str(), "Style configuration file is not the expected value.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1.0f, sc->getClockMultiplier(), "Clock multiplier is not the expected default value.");
    BatteryMonitorConfiguration& bmc = sc->getBatteryMonitorConfiguration();
    TEST_ASSERT_FALSE_MESSAGE(bmc.isEnabled(), "BatteryMonitorConfiguration should be disabled by default.");
    PowerLedConfiguration& plc = sc->getPowerLedConfiguration();
    TEST_ASSERT_FALSE_MESSAGE(plc.isEnabled(), "PowerLedConfiguration should be disabled by default.");
    Tm1637DisplayConfiguration& tdc = sc->getTm1637DisplayConfiguration();
    TEST_ASSERT_FALSE_MESSAGE(tdc.isEnabled(), "Tm1637DisplayConfiguration should be disabled by default.");
    BluetoothConfiguration& btc = sc->getBluetoothConfiguration();
    TEST_ASSERT_TRUE_MESSAGE(btc.isEnabled(), "BluetoothConfiguration should be enabled by default.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("99be4fac-c708-41e5-a149-74047f554cc1", btc.getUuid().c_str(), "Bluetooth UUID is not the expected default value.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("LED Sign Controller", btc.getLocalName().c_str(), "Bluetooth local name is not the expected default value.");
    delete sc;
}

void buttonsAndActionsAreParsed() {
    String json = buttonsWithActions();
    SystemConfiguration* sc = SystemConfiguration::ParseJson(
        json.c_str(), 
        json.length(), 
        mockButtonFactory);
    
    TEST_ASSERT_TRUE_MESSAGE(sc->isValid(), "SystemConfiguration should be valid.");
    ButtonProcessor& bp = sc->getButtonProcessor();
    std::vector<GenericButton*> buttons = bp.getButtons();
    TEST_ASSERT_EQUAL_MESSAGE(2, buttons.size(), "There should be 2 buttons.");
    bp.setActionProcessor(processAction);

    // We don't need to test all functionality of the ButtonProcessor here,
    // just that the buttons and actions were parsed correctly.

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

    // Simulate buttons 1 and 2 tap
    button1->setPressType(ButtonPressType::Normal);
    button2->setPressType(ButtonPressType::Normal);
    sc->getButtonProcessor().update();
    TEST_ASSERT_EQUAL_STRING_MESSAGE("changeStyle", lastActionName.c_str(), "Last action name is not correct for buttons 1 and 2 tap.");
    TEST_ASSERT_EQUAL_MESSAGE(1, lastArguments.size(), "Number of arguments is not correct for buttons 1 and 2 tap.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Fire", lastArguments[0].c_str(), "First argument is not correct for buttons 1 and 2 tap.");
    resetActionParameters();

    delete sc;
}

void disabledButtonDefinitionIsIgnored() {
    String json = disabledButton();
    SystemConfiguration* sc = SystemConfiguration::ParseJson(
        json.c_str(), 
        json.length(), 
        mockButtonFactory);
    
    TEST_ASSERT_TRUE_MESSAGE(sc->isValid(), "SystemConfiguration should be valid.");
    ButtonProcessor& bp = sc->getButtonProcessor();
    std::vector<GenericButton*> buttons = bp.getButtons();
    TEST_ASSERT_EQUAL_MESSAGE(1, buttons.size(), "There should be 1 button (the disabled one should be ignored).");
    bp.setActionProcessor(processAction);

    // Simulate button 1 tap (make sure the correct button was mapped)
    button1->setPressType(ButtonPressType::Normal);
    sc->getButtonProcessor().update();
    TEST_ASSERT_EQUAL_STRING_MESSAGE("changeStyle", lastActionName.c_str(), "Last action name is not correct for button 1 tap.");
    TEST_ASSERT_EQUAL_MESSAGE(2, lastArguments.size(), "Number of arguments is not correct for button 1 tap.");
    resetActionParameters();

    delete sc;
}

void additionalConfigurationsAreParsed() {
    String json = additionalConfigurations();
    SystemConfiguration* sc = SystemConfiguration::ParseJson(
        json.c_str(), 
        json.length(), 
        mockButtonFactory);
    
    TEST_ASSERT_TRUE_MESSAGE(sc->isValid(), "SystemConfiguration should be valid.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1.15f, sc->getClockMultiplier(), "Clock multiplier is not the expected value.");
    BatteryMonitorConfiguration& bmc = sc->getBatteryMonitorConfiguration();
    TEST_ASSERT_TRUE_MESSAGE(bmc.isEnabled(), "BatteryMonitorConfiguration should be enabled.");
    TEST_ASSERT_EQUAL_MESSAGE(29, bmc.getAnalogInputPin(), "BatteryMonitorConfiguration analog input pin is not correct.");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, 4.83f, bmc.getInputMultiplier(), "BatteryMonitorConfiguration input multiplier is not correct.");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, 6.7f, bmc.getVoltageToEnterLowPowerState(), "BatteryMonitorConfiguration voltage to enter low power state is not correct.");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, 7.2f, bmc.getVoltageToExitLowPowerState(), "BatteryMonitorConfiguration voltage to exit low power state is not correct.");
    PowerLedConfiguration& plc = sc->getPowerLedConfiguration();
    TEST_ASSERT_TRUE_MESSAGE(plc.isEnabled(), "PowerLedConfiguration should be enabled.");
    TEST_ASSERT_EQUAL_MESSAGE(22, plc.getGpioPin(), "PowerLedConfiguration GPIO pin is not correct.");
    Tm1637DisplayConfiguration& tdc = sc->getTm1637DisplayConfiguration();
    TEST_ASSERT_TRUE_MESSAGE(tdc.isEnabled(), "Tm1637DisplayConfiguration should be enabled.");
    TEST_ASSERT_EQUAL_MESSAGE(15, tdc.getClockGpioPin(), "Tm1637DisplayConfiguration clock GPIO pin is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(16, tdc.getDataGpioPin(), "Tm1637DisplayConfiguration data GPIO pin is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(5, tdc.getBrightness(), "Tm1637DisplayConfiguration brightness is not correct.");
    BluetoothConfiguration& btc = sc->getBluetoothConfiguration();
    TEST_ASSERT_FALSE_MESSAGE(btc.isEnabled(), "BluetoothConfiguration should be disabled.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("a32090db-3b1f-44f6-8b37-37a28b1a44dd", btc.getUuid().c_str(), "Bluetooth UUID is not correct.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("My LED Sign", btc.getLocalName().c_str(), "Bluetooth local name is not correct.");
    delete sc;
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(inValidJsonIsInvalid);
    RUN_TEST(emptyJsonDoesNothing);
    RUN_TEST(emptyJsonObjectDoesNothing);
    RUN_TEST(minimalJsonSetsProperties);
    RUN_TEST(buttonsAndActionsAreParsed);
    RUN_TEST(disabledButtonDefinitionIsIgnored);
    RUN_TEST(additionalConfigurationsAreParsed);

    return UNITY_END();
}

const char* minimalConfigurationJson() {
    return R"json(
        {
            "displayConfigurationFile": "display.json",
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

const char* additionalConfigurations()
{
    return R"json(
        {
            "bluetooth": {
                "enabled": false,
                "uuid": "a32090db-3b1f-44f6-8b37-37a28b1a44dd",
                "localName": "My LED Sign"
            },
            "batteryMonitor": {
                "enabled": true,
                "analogInputGpioPin": 29,
                "inputMultiplier": 4.83,
                "voltageToEnterLowPowerState": 6.7,
                "voltageToExitLowPowerState": 7.2
            },
            "powerLed": {
                "enabled": true,
                "gpioPin": 22
            },
            "clockMultiplier": 1.15,
            "tm1637Display": {
                "enabled": true,
                "clockGpioPin": 15,
                "dataGpioPin": 16,
                "brightness": 5
            }
        }
    )json";
}