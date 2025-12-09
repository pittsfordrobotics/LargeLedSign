#include "Tm1637DisplayConfiguration.h"

Tm1637DisplayConfiguration Tm1637DisplayConfiguration::fromJson(JsonVariant displayConfig)
{
    Tm1637DisplayConfiguration config;
    config.m_enabled = JsonUtils::getValueOrDefault<bool>(displayConfig, "enabled", config.m_enabled);
    config.m_clockGpioPin = JsonUtils::getValueOrDefault<int>(displayConfig, "clockGpioPin", config.m_clockGpioPin);
    config.m_dataGpioPin = JsonUtils::getValueOrDefault<int>(displayConfig, "dataGpioPin", config.m_dataGpioPin);
    config.m_brightness = JsonUtils::getValueOrDefault<byte>(displayConfig, "brightness", config.m_brightness);

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