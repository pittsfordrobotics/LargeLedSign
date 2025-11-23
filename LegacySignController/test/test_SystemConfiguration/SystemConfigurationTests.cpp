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
    if (button1 != nullptr) {
        delete button1;
        button1 = nullptr;
    }
    if (button2 != nullptr) {
        delete button2;
        button2 = nullptr;
    }
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
        processAction, 
        mockButtonFactory);
    
    TEST_ASSERT_NOT_NULL_MESSAGE(sc, "SystemConfiguration was null.");
    std::vector<GenericButton*> buttons = sc->getButtonProcessor().getButtons();
    TEST_ASSERT_EQUAL_MESSAGE(0, buttons.size(), "There should be no buttons.");
}

void emptyJsonObjectDoesNothing() {
    String json = "{}";
    SystemConfiguration* sc = SystemConfiguration::ParseJson(
        json.c_str(), 
        json.length(), 
        processAction, 
        mockButtonFactory);
    
    TEST_ASSERT_NOT_NULL_MESSAGE(sc, "SystemConfiguration was null.");
    std::vector<GenericButton*> buttons = sc->getButtonProcessor().getButtons();
    TEST_ASSERT_EQUAL_MESSAGE(0, buttons.size(), "There should be no buttons.");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(emptyJsonDoesNothing);
    RUN_TEST(emptyJsonObjectDoesNothing);
    
    return UNITY_END();
}

const char* oneButtonWithActions() {
    return "{"
    "    \"buttons\": {"
    "        \"definitions\": ["
    "            {"
    "                \"id\": \"1\","
    "                \"enabled\": true,"
    "                \"gpioPin\": 10"
    "            },"
    "            {"
    "                \"id\": \"2\","
    "                \"enabled\": true,"
    "                \"gpioPin\": 13"
    "            },"
    "            {"
    "                \"id\": \"power\","
    "                \"enabled\": true,"
    "                \"gpioPin\": 11"
    "            }"
    "        ],"
    "        \"actions\": ["
    "            {"
    "                \"buttonIds\": [\"1\"],"
    "                \"tapAction\": \"changeStyle\","
    "                \"longTapAction\": \"batteryVoltage\","
    "                \"tapActionArguments\": [\"Pink\", \"RainbowLava\"],"
    "                \"longTapActionArguments\": []"
    "            },"
    "            {"
    "                \"buttonId\": [\"2\"],"
    "                \"tapAction\": \"changeStyle\","
    "                \"tapActionArguments\": [\"Red\", \"Blue\"]"
    "            },"
    "            {"
    "                \"buttonId\": [\"power\"],"
    "                \"tapAction\": \"powerCycle\""
    "            },"
    "            {"
    "                \"enabled\": true,"
    "                \"buttonIds\": [\"1\", \"2\"]"
    "                \"tapAction\": \"\","
    "                \"longTapAction\": \"\""
    "            }"
    "        ]"
    "    }"
    "}";
}

const char* largeTest() {
    return "{"
    "    \"buttons\": {"
    "        \"definitions\": ["
    "            {"
    "                \"id\": \"1\","
    "                \"enabled\": true,"
    "                \"gpioPin\": 10"
    "            },"
    "            {"
    "                \"id\": \"2\","
    "                \"enabled\": true,"
    "                \"gpioPin\": 13"
    "            },"
    "            {"
    "                \"id\": \"power\","
    "                \"enabled\": true,"
    "                \"gpioPin\": 11"
    "            }"
    "        ],"
    "        \"actions\": ["
    "            {"
    "                \"buttonIds\": [\"1\"],"
    "                \"tapAction\": \"changeStyle\","
    "                \"longTapAction\": \"batteryVoltage\","
    "                \"tapActionArguments\": [\"Pink\", \"RainbowLava\"],"
    "                \"longTapActionArguments\": []"
    "            },"
    "            {"
    "                \"buttonId\": [\"2\"],"
    "                \"tapAction\": \"changeStyle\","
    "                \"tapActionArguments\": [\"Red\", \"Blue\"]"
    "            },"
    "            {"
    "                \"buttonId\": [\"power\"],"
    "                \"tapAction\": \"powerCycle\""
    "            },"
    "            {"
    "                \"enabled\": true,"
    "                \"buttonIds\": [\"1\", \"2\"]"
    "                \"tapAction\": \"\","
    "                \"longTapAction\": \"\""
    "            }"
    "        ]"
    "    }"
    "}";
}


/*
{
    "buttons": {
        "definitions": [
            {
                "id": "1",
                "enabled": true,
                "gpioPin": 10
            },
            {
                "id": "2",
                "enabled": true,
                "gpioPin": 13
            },
            {
                "id": "power",
                "enabled": true,
                "gpioPin": 11
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
                "buttonId": ["2"],
                "tapAction": "changeStyle",
                "tapActionArguments": ["Red", "Blue"]
            },
            {
                "buttonId": ["power"],
                "tapAction": "powerCycle"
            },
            {
                "enabled": true,
                "buttonIds": ["1", "2"]
                "tapAction": "",
                "longTapAction": ""
            }
        ]
    },
    "batteryMonitor": {
        "enabled": true,
        "analogInputPin": "1",   // gpio or analog number?
        "inputMultiplier": "3.14",
        "voltageToEnterLowPowerState": 6.2,
        "voltageToExitLowPowerState": 8.1
    },
    "powerLed": {
        "enabled": true,
        "gpioPin": 22
    },
    "clockMultiplier": 1.0,
    "tm1637Display": {
        "enabled": true,
        "clockGpioPin": 15,
        "dataGpioPin": 16
    }
}

*/

