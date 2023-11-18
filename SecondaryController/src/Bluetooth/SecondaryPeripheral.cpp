#include "SecondaryPeripheral.h"

SecondaryPeripheral::SecondaryPeripheral()
{
    m_additionalCharacteristics.push_back(m_signDataCharacteristic);
}

String SecondaryPeripheral::getSignData()
{
    m_currentSignData = readStringFromCharacteristic(m_signDataCharacteristic, m_currentSignData, "SignData");
    return m_currentSignData;
}

void SecondaryPeripheral::setSignData(String signData)
{
    Serial.print("Setting sign data to ");
    Serial.println(signData);
    m_currentSignData = signData;
    m_signDataCharacteristic.writeValue(signData);
}