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

    ServiceStatus status = getServiceStatus();
    SignConfigurationData signConfigData = status.getSignConfigurationData();
    if (signConfigData.getSignOrder() < 0 
        || signConfigData.getSignType() < 0
        || signConfigData.getColumnCount() < 0 
        || signConfigData.getPixelCount() < 0) {
            Serial.println("Sign data is not valid - disconecting.");
            disconnect();
            return;
        }
    
    m_signOrder = signConfigData.getSignOrder();
    m_isValid = true;
}

float SecondaryClient::getBatteryVoltage() {
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

ServiceStatus SecondaryClient::getServiceStatus() {
    ServiceStatus status;
    status.setBrightness(getByteValue(BTCOMMON_BRIGHTNESSCHARACTERISTIC_UUID));
    status.setPattern(getByteValue(BTCOMMON_PATTERNCHARACTERISTIC_UUID));
    status.setPatternNames(getStringValue(BTCOMMON_PATTERNNAMESCHARACTERISTIC_UUID));
    status.setSpeed(getByteValue(BTCOMMON_SPEEDCHARACTERISTIC_UUID));
    status.setStep(getByteValue(BTCOMMON_STEPCHARACTERISTIC_UUID));
    status.setStyle(getByteValue(BTCOMMON_STYLECHARACTERISTIC_UUID));
    status.setStyleNames(getStringValue(BTCOMMON_STYLENAMESCHARACTERISTIC_UUID));
    String signConfigData = getStringValue(BTCOMMON_SIGNCONFIGURATION_CHARACTERISTIC_UUID);
    status.setSignConfigurationData(SignConfigurationData(signConfigData));

    return status;
}

void SecondaryClient::setBrightness(byte brightness) {
    m_peripheral.characteristic(BTCOMMON_BRIGHTNESSCHARACTERISTIC_UUID).writeValue(brightness);
}

void SecondaryClient::setPattern(byte pattern) {
    m_peripheral.characteristic(BTCOMMON_PATTERNCHARACTERISTIC_UUID).writeValue(pattern);
}

void SecondaryClient::setStyle(byte style) {
    m_peripheral.characteristic(BTCOMMON_STYLECHARACTERISTIC_UUID).writeValue(style);
}

void SecondaryClient::setSpeed(byte speed) {
    m_peripheral.characteristic(BTCOMMON_SPEEDCHARACTERISTIC_UUID).writeValue(speed);
}

void SecondaryClient::setStep(byte step) {
    m_peripheral.characteristic(BTCOMMON_STEPCHARACTERISTIC_UUID).writeValue(step);
}

void SecondaryClient::setSignOffsetData(String offsetData) {
    m_peripheral.characteristic(BTCOMMON_OFFSETDATA_CHARACTERISTIC_UUID).writeValue(offsetData.c_str());
}

void SecondaryClient::updateSyncData(ulong syncData) {
    m_peripheral.characteristic(BTCOMMON_SYNCDATA_CHARACTERISTIC_UUID).writeValue(syncData);
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

float SecondaryClient::getFloatValue(String characteristicUuid) {
    BLECharacteristic characteristic = m_peripheral.characteristic(characteristicUuid.c_str());
    float value;
    memcpy(&value, (unsigned char*)characteristic.value(), characteristic.valueSize());

    return value;
}

SecondaryClient::~SecondaryClient()
{
    disconnect();
}