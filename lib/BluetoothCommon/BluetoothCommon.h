#include <Arduino.h>
#include <BLETypedCharacteristics.h>
#include <ArduinoBLE.h>
#include <vector>

#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#define BTCOMMON_PRIMARYCONTROLLER_UUID "99be4fac-c708-41e5-a149-74047f554cc1"
#define BTCOMMON_SECONDARYCONTROLLER_UUID "1221ca8d-4172-4946-bcd1-f9e4b40ba6b0"

#define BTCOMMON_BRIGHTNESSCHARACTERISTIC_UUID "5eccb54e-465f-47f4-ac50-6735bfc0e730"
#define BTCOMMON_STYLECHARACTERISTIC_UUID "c99db9f7-1719-43db-ad86-d02d36b191b3"
#define BTCOMMON_STYLENAMESCHARACTERISTIC_UUID "9022a1e0-3a1f-428a-bad6-3181a4d010a5"
#define BTCOMMON_SPEEDCHARACTERISTIC_UUID "b975e425-62e4-4b08-a652-d64ad5097815"
#define BTCOMMON_STEPCHARACTERISTIC_UUID "70e51723-0771-4946-a5b3-49693e9646b5"
#define BTCOMMON_PATTERNCHARACTERISTIC_UUID "6b503d25-f643-4823-a8a6-da51109e713f"
#define BTCOMMON_PATTERNNAMESCHARACTERISTIC_UUID "348195d1-e237-4b0b-aea4-c818c3eb5e2a"
#define BTCOMMON_BATTERYVOLTAGECHARACTERISTIC_UUID "ea0a95bc-7561-4b1e-8925-7973b3ad7b9a"

#define BLUETOOTH_H_MAXSTRINGLENGTH 250

class BluetoothCommon {
  public:
    void initialize(String uuid, String localName);
    void initialize(String uuid, String localName, std::vector<BLECharacteristic> additionalCharacteristics);
    void stop();
    void resume();

    void setStyleNames(std::vector<String> styleNames);
    void setPatternNames(std::vector<String> patternNames);
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

    void emitBatteryVoltage(float voltage);

  private:
    BLEService* m_ledService;
    BLEByteCharacteristic m_brightnessCharacteristic{ BTCOMMON_BRIGHTNESSCHARACTERISTIC_UUID, BLERead | BLENotify | BLEWrite };
    BLEByteCharacteristic m_styleCharacteristic{ BTCOMMON_STYLECHARACTERISTIC_UUID, BLERead | BLENotify | BLEWrite };
    BLEStringCharacteristic m_styleNamesCharacteristic{ BTCOMMON_STYLENAMESCHARACTERISTIC_UUID, BLERead, BLUETOOTH_H_MAXSTRINGLENGTH };
    BLEByteCharacteristic m_speedCharacteristic{BTCOMMON_SPEEDCHARACTERISTIC_UUID, BLERead | BLENotify | BLEWrite };
    BLEByteCharacteristic m_stepCharacteristic{ BTCOMMON_STEPCHARACTERISTIC_UUID, BLERead | BLENotify | BLEWrite };
    BLEByteCharacteristic m_patternCharacteristic{ BTCOMMON_PATTERNCHARACTERISTIC_UUID, BLERead | BLENotify | BLEWrite };
    BLEStringCharacteristic m_patternNamesCharacteristic{ BTCOMMON_PATTERNNAMESCHARACTERISTIC_UUID, BLERead, BLUETOOTH_H_MAXSTRINGLENGTH };
    BLEFloatCharacteristic m_batteryVoltageCharacteristic{ BTCOMMON_BATTERYVOLTAGECHARACTERISTIC_UUID, BLERead | BLENotify };

    byte m_currentBrightness{0};
    byte m_currentStyle{0};
    byte m_currentPattern{0};
    byte m_currentSpeed{0};
    byte m_currentStep{0};

    String* joinStrings(std::vector<String> strings);
    byte readByteFromCharacteristic(BLEByteCharacteristic characteristic, byte defaultValue, String name);
};

#endif