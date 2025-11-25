#include "SystemConfiguration.h"

#include "SystemConfiguration.h"

SystemConfiguration* SystemConfiguration::ParseJson(
    const char* jsonString, 
    size_t length, 
    ButtonFactory buttonFactory) 
{
    SystemConfiguration* config = new SystemConfiguration();

    JsonDocument configDoc;
    DeserializationError err = deserializeJson(configDoc, jsonString, length);
    
    if (err == DeserializationError::EmptyInput)
    {
        DebugUtils::println("System configuration JSON string was empty.");
        return config;
    }

    if (err != DeserializationError::Ok)
    {
        DebugUtils::print("Error parsing system configuration JSON: ");
        DebugUtils::println(err.c_str());
        return config;
    }

    if (configDoc.isNull())
    {
        DebugUtils::println("System configuration JSON was null after parsing.");
        return config;
    }

    if (configDoc["displayConfigurationFile"].is<JsonVariant>())
    {
        std::string displayConfigFile = configDoc["displayConfigurationFile"].as<std::string>();
        config->m_displayConfigurationFile = String(displayConfigFile.c_str());
    }

    if (configDoc["styleConfigurationFile"].is<JsonVariant>())
    {
        std::string styleConfigFile = configDoc["styleConfigurationFile"].as<std::string>();
        config->m_styleConfigurationFile = String(styleConfigFile.c_str());
    }

    JsonVariant buttonConfigs = configDoc["buttons"].as<JsonVariant>();
    if (buttonFactory != nullptr && !buttonConfigs.isNull())
    {
        config->configureButtonProcessor(buttonConfigs, buttonFactory);
    }

    if (configDoc["clockMultiplier"].is<JsonVariant>())
    {
        config->m_clockMultiplier = configDoc["clockMultiplier"].as<float>();
    }

    if (configDoc["powerLed"].is<JsonVariant>())
    {
        config->m_powerLedConfiguration = 
            config->parsePowerLedConfiguration(configDoc["powerLed"].as<JsonVariant>());
    }

    if (configDoc["batteryMonitor"].is<JsonVariant>())
    {
        config->m_batteryMonitorConfiguration = 
            config->parseBatteryMonitorConfiguration(configDoc["batteryMonitor"].as<JsonVariant>());
    }

    if (configDoc["tm1637Display"].is<JsonVariant>())
    {
        config->m_tm1637DisplayConfiguration = 
            config->parseTm1637DisplayConfiguration(configDoc["tm1637Display"].as<JsonVariant>());
    }

    if (configDoc["bluetooth"].is<JsonVariant>())
    {
        config->m_bluetoothConfiguration = 
            config->parseBluetoothConfiguration(configDoc["bluetooth"].as<JsonVariant>());
    }

    config->m_isValid = true;
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

PowerLedConfiguration SystemConfiguration::parsePowerLedConfiguration(JsonVariant plcVariant)
{
    PowerLedConfiguration defaultPlc;
    bool enabled = defaultPlc.isEnabled();
    int gpioPin = defaultPlc.getGpioPin();
    
    if (plcVariant["enabled"].is<JsonVariant>())
    {
        enabled = plcVariant["enabled"].as<bool>();
    }

    if (plcVariant["gpioPin"].is<JsonVariant>())
    {
        gpioPin = plcVariant["gpioPin"].as<int>();
    }

    return PowerLedConfiguration(enabled, gpioPin);
}

BatteryMonitorConfiguration SystemConfiguration::parseBatteryMonitorConfiguration(JsonVariant bmcVariant)
{
    BatteryMonitorConfiguration defaultBmc;
    bool enabled = defaultBmc.isEnabled();
    int analogInputPin = defaultBmc.getAnalogInputPin();
    float inputMultiplier = defaultBmc.getInputMultiplier();
    float voltageToEnterLowPowerState = defaultBmc.getVoltageToEnterLowPowerState();
    float voltageToExitLowPowerState = defaultBmc.getVoltageToExitLowPowerState();
    
    if (bmcVariant["enabled"].is<JsonVariant>())
    {
        enabled = bmcVariant["enabled"].as<bool>();
    }

    if (bmcVariant["analogInputGpioPin"].is<JsonVariant>())
    {
        analogInputPin = bmcVariant["analogInputGpioPin"].as<int>();
    }

    if (bmcVariant["inputMultiplier"].is<JsonVariant>())
    {
        inputMultiplier = bmcVariant["inputMultiplier"].as<float>();
    }

    if (bmcVariant["voltageToEnterLowPowerState"].is<JsonVariant>())
    {
        voltageToEnterLowPowerState = bmcVariant["voltageToEnterLowPowerState"].as<float>();
    }

    if (bmcVariant["voltageToExitLowPowerState"].is<JsonVariant>())
    {
        voltageToExitLowPowerState = bmcVariant["voltageToExitLowPowerState"].as<float>();
    }

    return BatteryMonitorConfiguration(
        enabled,
        analogInputPin,
        inputMultiplier,
        voltageToEnterLowPowerState,
        voltageToExitLowPowerState);
}

Tm1637DisplayConfiguration SystemConfiguration::parseTm1637DisplayConfiguration(JsonVariant tdcVariant)
{
    Tm1637DisplayConfiguration defaultTdc;
    bool enabled = defaultTdc.isEnabled();
    int clockGpioPin = defaultTdc.getClockGpioPin();
    int dataGpioPin = defaultTdc.getDataGpioPin();
    int brightness = defaultTdc.getBrightness();
    
    if (tdcVariant["enabled"].is<JsonVariant>())
    {
        enabled = tdcVariant["enabled"].as<bool>();
    }

    if (tdcVariant["clockGpioPin"].is<JsonVariant>())
    {
        clockGpioPin = tdcVariant["clockGpioPin"].as<int>();
    }

    if (tdcVariant["dataGpioPin"].is<JsonVariant>())
    {
        dataGpioPin = tdcVariant["dataGpioPin"].as<int>();
    }

    if (tdcVariant["brightness"].is<JsonVariant>())
    {
        brightness = tdcVariant["brightness"].as<byte>();
    }

    return Tm1637DisplayConfiguration(
        enabled,
        clockGpioPin,
        dataGpioPin,
        brightness);
}

BluetoothConfiguration SystemConfiguration::parseBluetoothConfiguration(JsonVariant btcVariant)
{
    BluetoothConfiguration defaultBtc;
    bool enabled = defaultBtc.isEnabled();
    String uuid = defaultBtc.getUuid();
    String localName = defaultBtc.getLocalName();
    
    if (btcVariant["enabled"].is<JsonVariant>())
    {
        enabled = btcVariant["enabled"].as<bool>();
    }

    if (btcVariant["uuid"].is<JsonVariant>())
    {
        std::string uuidStr = btcVariant["uuid"].as<std::string>();
        uuid = String(uuidStr.c_str());
    }

    if (btcVariant["localName"].is<JsonVariant>())
    {
        std::string localNamestr = btcVariant["localName"].as<std::string>();
        localName = String(localNamestr.c_str());
    }

    return BluetoothConfiguration(
        enabled,
        uuid,
        localName);
}
