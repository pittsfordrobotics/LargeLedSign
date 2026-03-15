# include "BatteryMonitor.h"

BatteryMonitor::BatteryMonitor(const BatteryMonitorConfiguration& configuration) : m_config(configuration)
{
    if (m_config.isEnabled())
    {
        pinMode(m_config.getAnalogInputPin(), INPUT);
    }
}

int BatteryMonitor::getRawVoltageInputLevel()
{
    if (!m_config.isEnabled())
    {
        return -1;
    }

    return analogRead(m_config.getAnalogInputPin());
}

float BatteryMonitor::getCalculatedBatteryVoltage()
{
    if (!m_config.isEnabled())
    {
        return 0.0f;
    }

    // The analog input ranges from 0 (0V) to 1024 (3.3V), resulting in 0.00322 Volts per "tick".
    // The battery voltage passes through a voltage divider so we have to multiply the input by
    // a scale factor to get the actual voltage.
    float rawLevel = getRawVoltageInputLevel();
    return rawLevel * m_config.getInputMultiplier() * 3.3 / 1024;
}

bool BatteryMonitor::isBatteryBelowLowPowerThreshold()
{
    if (!m_config.isEnabled())
    {
        return false;
    }

    float voltage = getCalculatedBatteryVoltage();
    return voltage < m_config.getVoltageToEnterLowPowerState();
}

bool BatteryMonitor::isBatteryAboveRestartThreshold()
{
    if (!m_config.isEnabled())
    {
        return false;
    }

    float voltage = getCalculatedBatteryVoltage();
    return voltage > m_config.getVoltageToExitLowPowerState();
}
