#include "SystemConfiguration.h"

#include "SystemConfiguration.h"

SystemConfiguration* SystemConfiguration::ParseJson(
    const char* jsonString, 
    size_t length, 
    ButtonActionProcessor actionProcessor,
    ButtonFactory buttonFactory) 
{
    SystemConfiguration* config = new SystemConfiguration();

    JsonDocument configDoc;
    DeserializationError err = deserializeJson(configDoc, jsonString, length);
    
    if (err == DeserializationError::EmptyInput)
    {
        DebugUtils::println("Display configuration JSON string was empty.");
        return config;
    }

    if (err != DeserializationError::Ok)
    {
        DebugUtils::print("Error parsing display configuration JSON: ");
        DebugUtils::println(err.c_str());
        return config;
    }

    if (configDoc.isNull())
    {
        DebugUtils::println("Display configuration JSON was null after parsing.");
        return config;
    }

    JsonVariant buttonConfigs = configDoc["buttons"].as<JsonVariant>();
    if (buttonFactory != nullptr && !buttonConfigs.isNull())
    {
        config->m_buttonProcessor.setActionProcessor(actionProcessor);
        config->configureButtonProcessor(buttonConfigs, buttonFactory);
    }

    return config;
}

SystemConfiguration::~SystemConfiguration()
{
    // Clean up dynamically allocated buttons
    for (auto* button : m_buttons) {
        delete button;
    }
}

void SystemConfiguration::configureButtonProcessor(JsonVariant buttonConfigs, ButtonFactory buttonFactory) {
    if (!buttonConfigs["definitions"].is<JsonVariant>())
    {
        DebugUtils::println("Button definitions were missing.");
        return;
    }

    JsonArray definitions = buttonConfigs["definitions"].as<JsonArray>();
    if (definitions.isNull() || definitions.size() == 0)
    {
        DebugUtils::println("Button definitions were null or empty.");
        return;
    }

    addButtonDefinitions(definitions, buttonFactory);

    if (!buttonConfigs["actions"].is<JsonVariant>())
    {
        DebugUtils::println("Button actions were missing.");
        return;
    }

    JsonArray actions = buttonConfigs["actions"].as<JsonArray>();
    if (actions.isNull() || actions.size() == 0)
    {
        DebugUtils::println("Button actions were null or empty.");
        return;
    }

    addButtonActions(actions);
}

void SystemConfiguration::addButtonDefinitions(JsonArray definitions, ButtonFactory buttonFactory)
{
    for (JsonVariant definition : definitions)
    {
        if (definition["enabled"].is<JsonVariant>() && (!definition["enabled"].as<bool>()))
        {
            DebugUtils::println("Skipping disabled button definition.");
            continue;
        }
        
        if (!definition["id"].is<JsonVariant>() || !definition["gpioPin"].is<JsonVariant>())
        {
            DebugUtils::println("Skipping button definition with missing gpioPin.");
            continue;
        }

        String id = String(definition["id"].as<std::string>().c_str());
        int gpioPin = definition["gpioPin"].as<int>();
        GenericButton* button = buttonFactory(gpioPin);
        if (button != nullptr)
        {
            m_buttons.push_back(button);  // Track for cleanup
            m_buttonProcessor.addButtonDefinition(id, button);
        }
    }
}

void SystemConfiguration::addButtonActions(JsonArray actions)
{
    for (JsonVariant action : actions)
    {
        if (action["enabled"].is<JsonVariant>() && (!action["enabled"].as<bool>()))
        {
            DebugUtils::println("Skipping disabled button action.");
            continue;
        }

        if (!action["buttonIds"].is<JsonVariant>())
        {
            DebugUtils::println("Skipping button action with invalid ids.");
            continue;
        }

        std::vector<String> buttonIds = getStringList(action["buttonIds"]);
        if (buttonIds.size() == 0)
        {
            DebugUtils::println("Skipping button action with no valid ids.");
            continue;
        }

        String tapAction = "";
        std::vector<String> tapActionArgs;
        if (action["tapAction"].is<std::string>())
        {
            tapAction = String(action["tapAction"].as<std::string>().c_str());
            tapActionArgs = getStringList(action["tapActionArguments"]);
        }

        String longTapAction = "";
        std::vector<String> longTapActionArgs;
        if (action["longTapAction"].is<std::string>())
        {
            longTapAction = String(action["longTapAction"].as<std::string>().c_str());
            longTapActionArgs = getStringList(action["longTapActionArguments"]);
        }

        if (!tapAction.equals(""))
        {
            m_buttonProcessor.addTapAction(buttonIds, tapAction, tapActionArgs);
        }

        if (!longTapAction.equals(""))
        {
            m_buttonProcessor.addLongTapAction(buttonIds, longTapAction, longTapActionArgs);
        }
    }
}

std::vector<String> SystemConfiguration::getStringList(JsonVariant array)
{
    std::vector<String> result;
    if (!array.is<JsonArray>())
    {
        return result;
    }

    for (JsonVariant item : array.as<JsonArray>())
    {
        if (item.is<std::string>())
        {
            result.push_back(String(item.as<std::string>().c_str()));
        }
    }

    return result;
}

/*

systemconfiguration.json

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


"tapAction": "setStyle:styleList1" // if not specified, default to "styleList"
"longTapAction": "setStyle:styleList2"

Actions: none, setStyle, powerCycle, batteryVoltage, ...
button combos?


hardware.json (system.json?)

power indicator led pin (if present)
4-char display setup
clock multiplier
battery levels - input pin, high/low levels, multiplier

buttons: {
    definitions: [],
    combinations: []
}

*/