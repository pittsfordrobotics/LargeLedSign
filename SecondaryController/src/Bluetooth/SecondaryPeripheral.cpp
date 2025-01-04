#include "SecondaryPeripheral.h"

SecondaryPeripheral::SecondaryPeripheral()
{
    //m_additionalCharacteristics.push_back(m_signConfigurationCharacteristic);
    //m_additionalCharacteristics.push_back(m_signOffsetDataCharacteristic);
    m_additionalCharacteristics.push_back(m_syncDataCharacteristic);
    m_additionalCharacteristics.push_back(m_timestampCharacteristic);

    m_syncDataCharacteristic.writeValue(0);
}

// void SecondaryPeripheral::setSignConfigurationData(const SignConfigurationData& data)
// {
//     m_signConfigurationCharacteristic.writeValue(&data, sizeof(data));
// }

// SignOffsetData SecondaryPeripheral::getSignOffsetData()
// {
//     m_signOffsetDataCharacteristic.readValue(&m_currentOffsetData, sizeof(m_currentOffsetData));
//     return m_currentOffsetData;
// }

ulong SecondaryPeripheral::getSyncData()
{
    m_currentSyncData = readULongFromCharacteristic(m_syncDataCharacteristic, m_currentSyncData, "SyncData");
    return m_currentSyncData;
}

void SecondaryPeripheral::emitTimestamp(ulong timestamp)
{
    m_timestampCharacteristic.writeValue(timestamp);
}