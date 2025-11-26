#include "PowerLedConfiguration.h"
PowerLedConfiguration::PowerLedConfiguration()
{
}

PowerLedConfiguration::PowerLedConfiguration(bool enabled,
    int gpioPin)
{
    m_enabled = enabled;
    m_dataGpioPin = gpioPin;
}

PowerLedConfiguration::PowerLedConfiguration(const PowerLedConfiguration& other)
{
    copy(other);
}

PowerLedConfiguration& PowerLedConfiguration::operator=(const PowerLedConfiguration& other)
{
    copy(other);

    return *this;
}

void PowerLedConfiguration::copy(const PowerLedConfiguration& other)
{
    this->m_enabled = other.m_enabled;
    this->m_dataGpioPin = other.m_dataGpioPin;
}