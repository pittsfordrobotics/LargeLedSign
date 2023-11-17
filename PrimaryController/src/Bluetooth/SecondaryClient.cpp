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

    ServiceStatus status = getServiceStatus();
    if (status.getSignOrder() < 0 || status.getSignType() < 0
        || status.getColumnCount() < 0 || status.getPixelCount() < 0) {
            Serial.println("Sign data is not valid - disconecting.");
            disconnect();
            return;
        }
    
    m_signOrder = status.getSignOrder();
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

ServiceStatus SecondaryClient::getServiceStatus() {
    ServiceStatus status;
    status.setBrightness(getByteValue(BTCOMMON_BRIGHTNESSCHARACTERISTIC_UUID));
    status.setPattern(getByteValue(BTCOMMON_PATTERNCHARACTERISTIC_UUID));
    status.setPatternNames(getStringValue(BTCOMMON_PATTERNNAMESCHARACTERISTIC_UUID));
    status.setSignData(getStringValue(BTCOMMON_SIGNDATACHARACTERISTIC_UUID));
    status.setSpeed(getByteValue(BTCOMMON_SPEEDCHARACTERISTIC_UUID));
    status.setStep(getByteValue(BTCOMMON_STEPCHARACTERISTIC_UUID));
    status.setStyle(getByteValue(BTCOMMON_STYLECHARACTERISTIC_UUID));
    status.setStyleNames(getStringValue(BTCOMMON_STYLENAMESCHARACTERISTIC_UUID));

    return status;
}

String SecondaryClient::getStringValue(String characteristicUuid) {
    BLECharacteristic characteristic = m_peripheral.characteristic(characteristicUuid.c_str());
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

byte SecondaryClient::getByteValue(String characteristicUuid) {
    BLECharacteristic characteristic = m_peripheral.characteristic(characteristicUuid.c_str());
    characteristic.read();
    byte value;
    characteristic.readValue(value);

    return value;
}

SecondaryClient::~SecondaryClient()
{
    disconnect();
}