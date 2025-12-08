#include "BluetoothConfiguration.h"

BluetoothConfiguration BluetoothConfiguration::fromJson(JsonVariant bluetoothConfig)
{
    BluetoothConfiguration config;
    config.m_enabled = JsonUtils::getValueOrDefault<bool>(bluetoothConfig, "enabled", config.m_enabled);
    config.m_uuid = JsonUtils::getValueOrDefault<String>(bluetoothConfig, "uuid", config.m_uuid);
    config.m_localName = JsonUtils::getValueOrDefault<String>(bluetoothConfig, "localName", config.m_localName);
    config.m_secondaryModeEnabled = JsonUtils::getValueOrDefault<bool>(bluetoothConfig, "isSecondaryModeEnabled", config.m_secondaryModeEnabled);
    config.m_proxyModeEnabled = JsonUtils::getValueOrDefault<bool>(bluetoothConfig, "isProxyModeEnabled", config.m_proxyModeEnabled);

    return config;
}

BluetoothConfiguration::BluetoothConfiguration()
{
}

BluetoothConfiguration::BluetoothConfiguration(const BluetoothConfiguration& other)
{
    copy(other);
}

BluetoothConfiguration& BluetoothConfiguration::operator=(const BluetoothConfiguration& other)
{
    copy(other);

    return *this;
}

void BluetoothConfiguration::copy(const BluetoothConfiguration& other)
{
    this->m_enabled = other.m_enabled;
    this->m_uuid = other.m_uuid;
    this->m_localName = other.m_localName;
    this->m_secondaryModeEnabled = other.m_secondaryModeEnabled;
    this->m_proxyModeEnabled = other.m_proxyModeEnabled;
}