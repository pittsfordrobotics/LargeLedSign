#include "SecondaryPeripheral.h"

SecondaryPeripheral::SecondaryPeripheral()
{
    m_additionalCharacteristics.push_back(m_signConfigurationCharacteristic);
    m_additionalCharacteristics.push_back(m_signOffsetDataCharacteristic);
    m_additionalCharacteristics.push_back(m_syncDataCharacteristic);
}

void SecondaryPeripheral::setSignConfigurationData(String signData)
{
    m_signConfigurationCharacteristic.writeValue(signData);
}

String SecondaryPeripheral::getSignOffsetData()
{
    m_currentOffsetData = readStringFromCharacteristic(m_signOffsetDataCharacteristic, m_currentOffsetData, "OffsetData");
    return m_currentOffsetData;
}

ulong SecondaryPeripheral::getSyncData()
{
    m_currentSyncData = readULongFromCharacteristic(m_syncDataCharacteristic, m_currentSyncData, "SyncData");
    return m_currentSyncData;
}

void SecondaryPeripheral::setSyncData(ulong syncData)
{
    m_currentSyncData = syncData;
    m_syncDataCharacteristic.writeValue(syncData);
}