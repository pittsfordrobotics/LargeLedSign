#include "BluetoothConfiguration.h"

BluetoothConfiguration BluetoothConfiguration::fromJson(JsonVariant bluetoothConfig)
{
    BluetoothConfiguration config;

    if (bluetoothConfig["enabled"].is<JsonVariant>())
    {
        config.m_enabled = bluetoothConfig["enabled"].as<bool>();
    }

    if (bluetoothConfig["uuid"].is<JsonVariant>())
    {
        std::string uuidStr = bluetoothConfig["uuid"].as<std::string>();
        config.m_uuid = String(uuidStr.c_str());
    }

    if (bluetoothConfig["localName"].is<JsonVariant>())
    {
        std::string localNamestr = bluetoothConfig["localName"].as<std::string>();
        config.m_localName = String(localNamestr.c_str());
    }

    if (bluetoothConfig["isSecondaryModeEnabled"].is<JsonVariant>())
    {
        config.m_secondaryModeEnabled = bluetoothConfig["isSecondaryModeEnabled"].as<bool>();
    }

    if (bluetoothConfig["isProxyModeEnabled"].is<JsonVariant>())
    {
        config.m_proxyModeEnabled = bluetoothConfig["isProxyModeEnabled"].as<bool>();
    }

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