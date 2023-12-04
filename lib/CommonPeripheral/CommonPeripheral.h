#ifndef COMMON_PERIPHERAL_H
#define COMMON_PERIPHERAL_H

#include <Arduino.h>
#include <vector>
#include <ArduinoBLE.h>
#include <BLETypedCharacteristics.h>
#include <BluetoothCommon.h>
#include <StringUtils.h>
#include <PatternData.h>

class CommonPeripheral {
  public:
    void initialize(String uuid, String localName);
    void stop();
    void resume();
    bool isConnected();

    void setStyleNames(std::vector<String> styleNames);
    void setStyleNames(String styleNames);
    void setPatternNames(std::vector<String> patternNames);
    void setPatternNames(String patternNames);
    void setBrightness(byte brightness);
    byte getBrightness();
    void setStyle(byte style);
    byte getStyle();
    void setPattern(byte pattern);
    byte getPattern();
    void setStep(byte step);
    byte getStep();
    void setSpeed(byte speed);
    byte getSpeed();
    PatternData getPatternData();
    void setPatternData(PatternData data);

    void emitBatteryVoltage(float voltage);

  protected:
    std::vector<BLECharacteristic> m_additionalCharacteristics;
    byte readByteFromCharacteristic(BLEByteCharacteristic characteristic, byte defaultValue, String name);
    ulong readULongFromCharacteristic(BLEUnsignedLongCharacteristic characteristic, ulong defaultValue, String name);
    String readStringFromCharacteristic(BLEStringCharacteristic characteristic, String defaultValue, String name);

  private:
    BLEService* m_ledService;
    BLEByteCharacteristic m_brightnessCharacteristic{ BTCOMMON_BRIGHTNESSCHARACTERISTIC_UUID, BLERead | BLENotify | BLEWrite };
    BLEByteCharacteristic m_styleCharacteristic{ BTCOMMON_STYLECHARACTERISTIC_UUID, BLERead | BLENotify | BLEWrite };
    BLEStringCharacteristic m_styleNamesCharacteristic{ BTCOMMON_STYLENAMESCHARACTERISTIC_UUID, BLERead, BTCOMMON_MAXSTRINGLENGTH };
    BLEByteCharacteristic m_speedCharacteristic{BTCOMMON_SPEEDCHARACTERISTIC_UUID, BLERead | BLENotify | BLEWrite };
    BLEByteCharacteristic m_stepCharacteristic{ BTCOMMON_STEPCHARACTERISTIC_UUID, BLERead | BLENotify | BLEWrite };
    BLEByteCharacteristic m_patternCharacteristic{ BTCOMMON_PATTERNCHARACTERISTIC_UUID, BLERead | BLENotify | BLEWrite };
    BLEStringCharacteristic m_patternNamesCharacteristic{ BTCOMMON_PATTERNNAMESCHARACTERISTIC_UUID, BLERead, BTCOMMON_MAXSTRINGLENGTH };
    BLEFloatCharacteristic m_batteryVoltageCharacteristic{ BTCOMMON_BATTERYVOLTAGECHARACTERISTIC_UUID, BLERead | BLENotify };
    BLECharacteristic m_patternDataCharacteristic{ BTCOMMON_PATTERNDATA_CHARACTERISTIC_UUID, BLERead | BLEWrite, 20 };

    byte m_currentBrightness{0};
    byte m_currentStyle{0};
    byte m_currentPattern{0};
    byte m_currentSpeed{0};
    byte m_currentStep{0};
    PatternData m_currentPatternData;
};

#endif