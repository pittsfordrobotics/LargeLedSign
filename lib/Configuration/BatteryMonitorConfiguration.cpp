#include "BatteryMonitorConfiguration.h"

BatteryMonitorConfiguration BatteryMonitorConfiguration::fromJson(JsonVariant batteryMonitorConfig)
{
    BatteryMonitorConfiguration config;

    if (batteryMonitorConfig["enabled"].is<JsonVariant>())
    {
        config.m_enabled = batteryMonitorConfig["enabled"].as<bool>();
    }

    if (batteryMonitorConfig["analogInputGpioPin"].is<JsonVariant>())
    {
        config.m_analogInputPin = batteryMonitorConfig["analogInputGpioPin"].as<int>();
    }

    if (batteryMonitorConfig["inputMultiplier"].is<JsonVariant>())
    {
        config.m_inputMultiplier = batteryMonitorConfig["inputMultiplier"].as<float>();
    }

    if (batteryMonitorConfig["voltageToEnterLowPowerState"].is<JsonVariant>())
    {
        config.m_voltageToEnterLowPowerState = batteryMonitorConfig["voltageToEnterLowPowerState"].as<float>();
    }

    if (batteryMonitorConfig["voltageToExitLowPowerState"].is<JsonVariant>())
    {
        config.m_voltageToExitLowPowerState = batteryMonitorConfig["voltageToExitLowPowerState"].as<float>();
    }

    return config;
}

BatteryMonitorConfiguration::BatteryMonitorConfiguration()
{
}

BatteryMonitorConfiguration::BatteryMonitorConfiguration(const BatteryMonitorConfiguration& other)
{
    copy(other);
}

BatteryMonitorConfiguration& BatteryMonitorConfiguration::operator=(const BatteryMonitorConfiguration& other)
{
    copy(other);

    return *this;
}
 
void BatteryMonitorConfiguration::copy(const BatteryMonitorConfiguration& other)
{
    this->m_enabled = other.m_enabled;
    this->m_analogInputPin = other.m_analogInputPin;
    this->m_inputMultiplier = other.m_inputMultiplier;
    this->m_voltageToEnterLowPowerState = other.m_voltageToEnterLowPowerState;
    this->m_voltageToExitLowPowerState = other.m_voltageToExitLowPowerState;
}