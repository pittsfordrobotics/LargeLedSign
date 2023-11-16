#include "SecondaryClient.h"

SecondaryClient::SecondaryClient(BLEDevice peripheral)
{
    m_peripheral = BLEDevice(peripheral);
    initialize();
}

void SecondaryClient::initialize()
{
    m_isValid = false;
    if (!m_peripheral.discoverAttributes())
    {
        Serial.println("Attribute discovery failed!");
        disconnect();
        return;
    }

    BLECharacteristic signDataCharacteristic = m_peripheral.characteristic(BTCOMMON_SIGNDATACHARACTERISTIC_UUID);

    if (!signDataCharacteristic)
    {
        Serial.print("Peripheral does not have a characteristic with id '");
        Serial.print(BTCOMMON_SIGNDATACHARACTERISTIC_UUID);
        Serial.println("'!");
        disconnect();
        return;
    }

    String signData = getStringValue(signDataCharacteristic);
    Serial.println("Sign data from peripheral: " + signData);
    // Sign data should be of the form <style/type>;<position/order>;<numColumns>;<numPixels>
    std::vector<String> splitSignData = StringUtils::splitString(signData, ';');
    Serial.println("Split data:");
    for (uint i = 0; i < splitSignData.size(); i++) {
        Serial.println(" -" + splitSignData.at(i));
    }

    if (splitSignData.size() < 4) {
        // Too little data!
        disconnect();
        return;
    }

    m_signType = splitSignData.at(0).toInt();
    m_signOrder = splitSignData.at(1).toInt();
    m_columnCount = splitSignData.at(2).toInt();
    m_pixelCount = splitSignData.at(3).toInt();
    m_isValid = true;
}

bool SecondaryClient::isConnected()
{
    return m_peripheral.connected();
}

void SecondaryClient::disconnect()
{
    if (m_peripheral.connected())
    {
        m_peripheral.disconnect();
    }
}

String SecondaryClient::getLocalName()
{
    return m_peripheral.localName();
}

String SecondaryClient::getStringValue(BLECharacteristic characteristic)
{
    characteristic.read();

    // Code taken from the ArduinoBLE source code for
    // how a BLEStringCharacteristic reads a String value.
    String str;
    int length = characteristic.valueLength();
    const uint8_t *val = characteristic.value();

    str.reserve(length);

    for (int i = 0; i < length; i++)
    {
        str += (char)val[i];
    }

    return str;
}

SecondaryClient::~SecondaryClient()
{
    disconnect();
}