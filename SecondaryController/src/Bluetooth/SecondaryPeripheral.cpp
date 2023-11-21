#include "SecondaryPeripheral.h"

SecondaryPeripheral::SecondaryPeripheral()
{
    m_additionalCharacteristics.push_back(m_signDataCharacteristic);
}

String SecondaryPeripheral::getSignConfigurationData()
{
    m_currentSignData = readStringFromCharacteristic(m_signDataCharacteristic, m_currentSignData, "SignData");
    return m_currentSignData;
}

void SecondaryPeripheral::setSignConfigurationData(String signData)
{
    Serial.print("Setting sign data to ");
    Serial.println(signData);
    m_currentSignData = signData;
    m_signDataCharacteristic.writeValue(signData);
}

ulong SecondaryPeripheral::getSyncData() {
    m_currentSyncData = readULongFromCharacteristic(m_syncDataCharacteristic, m_currentSyncData, "SyncData");
    return m_currentSyncData;
}

void SecondaryPeripheral::setSyncData(ulong syncData) {
    m_currentSyncData = syncData;
    m_syncDataCharacteristic.writeValue(syncData);
}