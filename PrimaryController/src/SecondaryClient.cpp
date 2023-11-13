#include "SecondaryClient.h"

SecondaryClient::SecondaryClient(BLEDevice *peripheral)
{
    m_peripheral = peripheral;
    initialize();
}

void SecondaryClient::initialize()
{
    if (!m_peripheral->discoverAttributes())
    {
        Serial.println("Attribute discovery failed!");
        disconnect();
    }

    BLECharacteristic signDataCharacteristic = m_peripheral->characteristic(BTCOMMON_SIGNDATACHARACTERISTIC_UUID);

    if (!signDataCharacteristic)
    {
        Serial.print("Peripheral does not have a characteristic with id '");
        Serial.print(BTCOMMON_SIGNDATACHARACTERISTIC_UUID);
        Serial.println("'!");
        disconnect();
        return;
    }

    if (!signDataCharacteristic.canRead()) {
        Serial.println("Can't read characteristic!");
        return;
    }

    signDataCharacteristic.read();

    String signData = getStringValue(signDataCharacteristic);
    Serial.println("Sign data from peripheral: " + signData);
    // Sign data should be of the form <position>;<numColumns>;<numPixels>
    std::vector<String> splitSignData = splitString(signData,';');
    Serial.println("Split data:");
    for (uint i = 0; i < splitSignData.size(); i++) {
        Serial.println(" -" + splitSignData.at(i));
    }
}

bool SecondaryClient::isConnected()
{
    return m_peripheral->connected();
}

void SecondaryClient::disconnect()
{
    if (m_peripheral->connected())
    {
        m_peripheral->disconnect();
    }
}

String SecondaryClient::getLocalName()
{
    return m_peripheral->localName();
}

String SecondaryClient::getStringValue(BLECharacteristic characteristic)
{
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

    Serial.print("String char length: ");
    Serial.println(length);
    Serial.print("String: ");
    Serial.println(str);

    return str;
}

std::vector<String> SecondaryClient::splitString(String input, char delimiter) {
    std::vector<String> splitResults;
    int lastPos = 0;
    int nextPos = input.indexOf(delimiter);
    while (nextPos > 0) {
        splitResults.push_back(input.substring(lastPos, nextPos - 1));
        lastPos = nextPos + 1;
        nextPos = input.indexOf(delimiter, nextPos + 1);
    }
    splitResults.push_back(input.substring(lastPos));

    return splitResults;
}

SecondaryClient::~SecondaryClient()
{
    disconnect();
    delete m_peripheral;
}