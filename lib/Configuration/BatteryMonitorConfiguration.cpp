#include "BatteryMonitorConfiguration.h"

BatteryMonitorConfiguration::BatteryMonitorConfiguration()
{
}

BatteryMonitorConfiguration::BatteryMonitorConfiguration(bool enabled,
    int analogInputPin,
    float inputMultiplier,
    float voltageToEnterLowPowerState,
    float voltageToExitLowPowerState)
{
    m_enabled = enabled;
    m_analogInputPin = analogInputPin;
    m_inputMultiplier = inputMultiplier;
    m_voltageToEnterLowPowerState = voltageToEnterLowPowerState;
    m_voltageToExitLowPowerState = voltageToExitLowPowerState;  
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