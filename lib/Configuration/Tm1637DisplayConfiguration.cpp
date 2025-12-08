#include "Tm1637DisplayConfiguration.h"

Tm1637DisplayConfiguration Tm1637DisplayConfiguration::fromJson(JsonVariant displayConfig)
{
    Tm1637DisplayConfiguration config;

    if (displayConfig["enabled"].is<JsonVariant>())
    {
        config.m_enabled = displayConfig["enabled"].as<bool>();
    }

    if (displayConfig["clockGpioPin"].is<JsonVariant>())
    {
        config.m_clockGpioPin = displayConfig["clockGpioPin"].as<int>();
    }

    if (displayConfig["dataGpioPin"].is<JsonVariant>())
    {
        config.m_dataGpioPin = displayConfig["dataGpioPin"].as<int>();
    }

    if (displayConfig["brightness"].is<JsonVariant>())
    {
        config.m_brightness = displayConfig["brightness"].as<byte>();
    }

    return config;
}

Tm1637DisplayConfiguration::Tm1637DisplayConfiguration()
{
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