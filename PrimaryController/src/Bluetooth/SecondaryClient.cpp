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

    BLECharacteristic signDataCharacteristic = m_peripheral.characteristic(BTCOMMON_SIGNCONFIGURATION_CHARACTERISTIC_UUID);

    if (!signDataCharacteristic)
    {
        Serial.print("Peripheral does not have a characteristic with id '");
        Serial.print(BTCOMMON_SIGNCONFIGURATION_CHARACTERISTIC_UUID);
        Serial.println("'!");
        disconnect();
        return;
    }

    SignStatus status = getSignStatus();
    SignConfigurationData signConfigData = status.signConfigurationData;
    
    // If all sign config values are 0, there's something strange going on.
    if (signConfigData.signOrder == 0 || signConfigData.signType == 0 || signConfigData.columnCount == 0 || signConfigData.pixelCount == 0)
    {
        Serial.println("Sign data is not valid - disconecting.");
        disconnect();
        return;
    }

    m_signOrder = signConfigData.signOrder;
    m_isValid = true;
}

float SecondaryClient::getBatteryVoltage()
{
    return getFloatValue(BTCOMMON_BATTERYVOLTAGECHARACTERISTIC_UUID);
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

SignStatus SecondaryClient::getSignStatus()
{
    SignStatus status;
    status.brightness = getByteValue(BTCOMMON_BRIGHTNESSCHARACTERISTIC_UUID);
    status.speed = getByteValue(BTCOMMON_SPEEDCHARACTERISTIC_UUID);
    SignConfigurationData signConfigData;
    m_peripheral.characteristic(BTCOMMON_SIGNCONFIGURATION_CHARACTERISTIC_UUID).readValue(&signConfigData, sizeof(signConfigData));
    status.signConfigurationData = signConfigData;

    return status;
}

void SecondaryClient::setBrightness(byte brightness)
{
    m_peripheral.characteristic(BTCOMMON_BRIGHTNESSCHARACTERISTIC_UUID).writeValue(brightness);
}

void SecondaryClient::setSpeed(byte speed)
{
    m_peripheral.characteristic(BTCOMMON_SPEEDCHARACTERISTIC_UUID).writeValue(speed);
}

void SecondaryClient::setSignOffsetData(const SignOffsetData& data)
{
    m_peripheral.characteristic(BTCOMMON_OFFSETDATA_CHARACTERISTIC_UUID).writeValue(&data, sizeof(data));
}

void SecondaryClient::setPatternData(const PatternData& data)
{
    BLECharacteristic characteristic = m_peripheral.characteristic(BTCOMMON_PATTERNDATA_CHARACTERISTIC_UUID);
    if (characteristic)
    {
        characteristic.writeValue(&data, sizeof(data));
    }
}

void SecondaryClient::updateSyncData(ulong syncData)
{
    m_peripheral.characteristic(BTCOMMON_SYNCDATA_CHARACTERISTIC_UUID).writeValue(syncData);
}

String SecondaryClient::getColorPatternList()
{
    return getStringValue(BTCOMMON_COLORPATTERNLIST_CHARACTERISTIC_UUID);
}

String SecondaryClient::getDisplayPatternList()
{
    return getStringValue(BTCOMMON_DISPLAYPATTERNLIST_CHARACTERISTIC_UUID);
}

long SecondaryClient::getTimestamp()
{
    return getLongValue(BTCOMMON_TIMESTAMP_CHARACTERISTIC_UUID);
}

String SecondaryClient::getStringValue(String characteristicUuid)
{
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

byte SecondaryClient::getByteValue(String characteristicUuid)
{
    BLECharacteristic characteristic = m_peripheral.characteristic(characteristicUuid.c_str());
    characteristic.read();
    byte value;
    characteristic.readValue(value);

    return value;
}

float SecondaryClient::getFloatValue(String characteristicUuid)
{
    BLECharacteristic characteristic = m_peripheral.characteristic(characteristicUuid.c_str());
    characteristic.read();
    float value;
    memcpy(&value, (unsigned char *)characteristic.value(), characteristic.valueSize());

    return value;
}

long SecondaryClient::getLongValue(String characteristicUuid)
{
    BLECharacteristic characteristic = m_peripheral.characteristic(characteristicUuid.c_str());
    characteristic.read();
    long value;
    characteristic.readValue(value);

    return value;
}

SecondaryClient::~SecondaryClient()
{
    disconnect();
}