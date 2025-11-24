#include "Tm1637DisplayConfiguration.h"

Tm1637DisplayConfiguration::Tm1637DisplayConfiguration()
{
}

Tm1637DisplayConfiguration::Tm1637DisplayConfiguration(bool enabled,
    int clockGpioPin,
    int dataGpioPin,
    byte brightness)
{
    m_enabled = enabled;
    m_clockGpioPin = clockGpioPin;
    m_dataGpioPin = dataGpioPin;
    m_brightness = brightness;
}

Tm1637DisplayConfiguration::Tm1637DisplayConfiguration(const Tm1637DisplayConfiguration& other)
{
    copy(other);
}

Tm1637DisplayConfiguration& Tm1637DisplayConfiguration::operator=(const Tm1637DisplayConfiguration& other)
{
    copy(other);

    return *this;
}

void Tm1637DisplayConfiguration::copy(const Tm1637DisplayConfiguration& other)
{
    this->m_enabled = other.m_enabled;
    this->m_clockGpioPin = other.m_clockGpioPin;
    this->m_dataGpioPin = other.m_dataGpioPin;
    this->m_brightness = other.m_brightness;
}