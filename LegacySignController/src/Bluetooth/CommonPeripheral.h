#ifndef COMMON_PERIPHERAL_H
#define COMMON_PERIPHERAL_H

#include <Arduino.h>
#include <vector>
#include <ArduinoBLE.h>
#include <PatternData.h>
#include "BluetoothCommon.h"

class CommonPeripheral {
  public:
    void initialize(String uuid, String localName);
    void stop();
    void resume();
    bool isConnected();
    void disconnect();

    void setBrightness(byte brightness);
    byte getBrightness();
    void setSpeed(byte speed);
    byte getSpeed();
    PatternData getPatternData();
    void setPatternData(const PatternData& data);
    
    void emitBatteryVoltage(float voltage);
    void setColorPatternList(String patternList);
    void setDisplayPatternList(String patternList);

  protected:
    std::vector<BLECharacteristic> m_additionalCharacteristics;
    byte readByteFromCharacteristic(BLEByteCharacteristic characteristic, byte defaultValue, String name);
    ulong readULongFromCharacteristic(BLEUnsignedLongCharacteristic characteristic, ulong defaultValue, String name);
    String readStringFromCharacteristic(BLEStringCharacteristic characteristic, String defaultValue, String name);

  private:
    BLEService* m_ledService;
    BLEByteCharacteristic m_brightnessCharacteristic{ BTCOMMON_BRIGHTNESSCHARACTERISTIC_UUID, BLERead | BLENotify | BLEWrite };
    BLEByteCharacteristic m_speedCharacteristic{BTCOMMON_SPEEDCHARACTERISTIC_UUID, BLERead | BLENotify | BLEWrite };
    BLEFloatCharacteristic m_batteryVoltageCharacteristic{ BTCOMMON_BATTERYVOLTAGECHARACTERISTIC_UUID, BLERead | BLENotify };
    BLECharacteristic m_patternDataCharacteristic{ BTCOMMON_PATTERNDATA_CHARACTERISTIC_UUID, BLERead | BLENotify | BLEWrite, sizeof(PatternData) };
    BLEStringCharacteristic m_colorPatternListCharacteristic{ BTCOMMON_COLORPATTERNLIST_CHARACTERISTIC_UUID, BLERead, BTCOMMON_MAXSTRINGLENGTH };
    BLEStringCharacteristic m_displayPatternListCharacteristic{ BTCOMMON_DISPLAYPATTERNLIST_CHARACTERISTIC_UUID, BLERead, BTCOMMON_MAXSTRINGLENGTH };
    
    byte m_currentBrightness{0};
    byte m_currentSpeed{0};
    PatternData m_currentPatternData;
};

#endif