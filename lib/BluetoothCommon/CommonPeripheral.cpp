#include "CommonPeripheral.h"

void CommonPeripheral::initialize(String uuid, String localName)
{
    Serial.print("Initializing BLE peripheral service for UUID ");
    Serial.print(uuid);
    Serial.println(".");

    m_ledService = new BLEService(uuid.c_str());
    BLE.setLocalName(localName.c_str());
    BLE.setAdvertisedService(*m_ledService);
    m_ledService->addCharacteristic(m_brightnessCharacteristic);
    m_ledService->addCharacteristic(m_speedCharacteristic);
    m_ledService->addCharacteristic(m_batteryVoltageCharacteristic);
    m_ledService->addCharacteristic(m_patternDataCharacteristic);
    m_ledService->addCharacteristic(m_colorPatternListCharacteristic);
    m_ledService->addCharacteristic(m_displayPatternListCharacteristic);

    for (uint i = 0; i < m_additionalCharacteristics.size(); i++)
    {
        Serial.print("Adding additional characteristic ");
        Serial.println(m_additionalCharacteristics.at(i).uuid());
        m_ledService->addCharacteristic(m_additionalCharacteristics.at(i));
    }

    BLE.addService(*m_ledService);
    BLE.advertise();
}

void CommonPeripheral::stop()
{
    // Disconnect any clients and stop advertising.
    BLE.disconnect();
    BLE.stopAdvertise();
}

void CommonPeripheral::resume()
{
    BLE.advertise();
}

bool CommonPeripheral::isConnected()
{
    BLEDevice central = BLE.central();
    if (central)
    {
        return true;
    }
    return false;
}

void CommonPeripheral::disconnect() {
    BLEDevice central = BLE.central();
    if (central)
    {
        central.disconnect();
    }
}

void CommonPeripheral::setColorPatternList(String patternList)
{
    m_colorPatternListCharacteristic.writeValue(patternList);
}

void CommonPeripheral::setDisplayPatternList(String patternList)
{
    m_displayPatternListCharacteristic.writeValue(patternList);
}

byte CommonPeripheral::getBrightness()
{
    m_currentBrightness = readByteFromCharacteristic(m_brightnessCharacteristic, m_currentBrightness, "brightness");
    return m_currentBrightness;
}

void CommonPeripheral::setBrightness(byte brightness)
{
    m_currentBrightness = brightness;
    m_brightnessCharacteristic.writeValue(brightness);
}

byte CommonPeripheral::getSpeed()
{
    m_currentSpeed = readByteFromCharacteristic(m_speedCharacteristic, m_currentSpeed, "speed");
    return m_currentSpeed;
}

void CommonPeripheral::setSpeed(byte speed)
{
    m_currentSpeed = speed;
    m_speedCharacteristic.writeValue(speed);
}

void CommonPeripheral::emitBatteryVoltage(float voltage)
{
    m_batteryVoltageCharacteristic.writeValue(voltage);
}

PatternData CommonPeripheral::getPatternData()
{
    if (isConnected())
    {
        if (m_patternDataCharacteristic.written())
        {
            m_patternDataCharacteristic.readValue(&m_currentPatternData, sizeof(m_currentPatternData));
        }
    }

    return m_currentPatternData;
}

void CommonPeripheral::setPatternData(const PatternData& data)
{
    m_currentPatternData = data;
    m_patternDataCharacteristic.writeValue(&data, sizeof(data));
}

byte CommonPeripheral::readByteFromCharacteristic(BLEByteCharacteristic characteristic, byte defaultValue, String name)
{
    if (isConnected())
    {
        if (characteristic.written())
        {
            byte valByte = characteristic.value();
            return valByte;
        }
    }

    return defaultValue;
}

String CommonPeripheral::readStringFromCharacteristic(BLEStringCharacteristic characteristic, String defaultValue, String name)
{
    if (isConnected())
    {
        if (characteristic.written())
        {
            String value = characteristic.value();
            return value;
        }
    }

    return defaultValue;
}

ulong CommonPeripheral::readULongFromCharacteristic(BLEUnsignedLongCharacteristic characteristic, ulong defaultValue, String name)
{
    if (isConnected())
    {
        if (characteristic.written())
        {
            ulong value = characteristic.value();
            return value;
        }
    }

    return defaultValue;
}
