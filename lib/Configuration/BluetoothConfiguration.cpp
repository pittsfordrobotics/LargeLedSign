#include "BluetoothConfiguration.h"

BluetoothConfiguration::BluetoothConfiguration()
{
}

BluetoothConfiguration::BluetoothConfiguration(bool isEnabled, String uuid, String localName)
{
    m_enabled = isEnabled;
    m_uuid = uuid;
    m_localName = localName;
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
}