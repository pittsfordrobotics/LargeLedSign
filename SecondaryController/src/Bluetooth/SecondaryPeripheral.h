#ifndef SECONDARYPERIPHERAL_H
#define SECONDARYPERIPHERAL_H

#include <Arduino.h>
#include <ArduinoBLE.h>
#include "BluetoothCommon.h"
#include "CommonPeripheral.h"

class SecondaryPeripheral : public CommonPeripheral {
  public:
    SecondaryPeripheral();
    void setSignConfigurationData(String signData);
    String getSignOffsetData();
    ulong getSyncData();
    void emitTimestamp(ulong timestamp);

  private:
    String m_currentOffsetData;
    ulong m_currentSyncData{0};
    BLEStringCharacteristic m_signConfigurationCharacteristic{ BTCOMMON_SIGNCONFIGURATION_CHARACTERISTIC_UUID, BLERead, BTCOMMON_MAXSTRINGLENGTH };
    BLEStringCharacteristic m_signOffsetDataCharacteristic{ BTCOMMON_OFFSETDATA_CHARACTERISTIC_UUID, BLERead | BLEWrite, BTCOMMON_MAXSTRINGLENGTH };
    BLEUnsignedLongCharacteristic m_syncDataCharacteristic{ BTCOMMON_SYNCDATA_CHARACTERISTIC_UUID, BLERead | BLEWrite };
    BLEUnsignedLongCharacteristic m_timestampCharacteristic{ BTCOMMON_TIMESTAMP_CHARACTERISTIC_UUID, BLERead };
};

#endif