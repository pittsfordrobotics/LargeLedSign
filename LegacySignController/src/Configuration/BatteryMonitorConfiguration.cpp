#include "BatteryMonitorConfiguration.h"

BatteryMonitorConfiguration BatteryMonitorConfiguration::fromJson(JsonVariant batteryMonitorConfig)
{
    BatteryMonitorConfiguration config;
    config.m_enabled = JsonUtils::getValueOrDefault<bool>(batteryMonitorConfig, "enabled", config.m_enabled);
    config.m_analogInputPin = JsonUtils::getValueOrDefault<int>(batteryMonitorConfig, "analogInputGpioPin", config.m_analogInputPin);
    config.m_inputMultiplier = JsonUtils::getValueOrDefault<float>(batteryMonitorConfig, "inputMultiplier", config.m_inputMultiplier);
    config.m_voltageToEnterLowPowerState = JsonUtils::getValueOrDefault<float>(batteryMonitorConfig, "voltageToEnterLowPowerState", config.m_voltageToEnterLowPowerState);
    config.m_voltageToExitLowPowerState = JsonUtils::getValueOrDefault<float>(batteryMonitorConfig, "voltageToExitLowPowerState", config.m_voltageToExitLowPowerState);

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