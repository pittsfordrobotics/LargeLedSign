#ifndef SECONDARYPERIPHERAL_H
#define SECONDARYPERIPHERAL_H

#include <Arduino.h>
#include <ArduinoBLE.h>
#include "BluetoothCommon.h"
#include "CommonPeripheral.h"
#include <SignConfigurationData.h>
#include <SignOffsetData.h>

class SecondaryPeripheral : public CommonPeripheral {
  public:
    SecondaryPeripheral();
    void setSignConfigurationData(const SignConfigurationData& data);
    SignOffsetData getSignOffsetData();
    ulong getSyncData();
    void emitTimestamp(ulong timestamp);

  private:
    SignOffsetData m_currentOffsetData;
    ulong m_currentSyncData{0};
    BLECharacteristic m_signConfigurationCharacteristic{ BTCOMMON_SIGNCONFIGURATION_CHARACTERISTIC_UUID, BLERead, sizeof(SignConfigurationData) };
    BLECharacteristic m_signOffsetDataCharacteristic{ BTCOMMON_OFFSETDATA_CHARACTERISTIC_UUID, BLERead | BLEWrite, sizeof(SignOffsetData) };
    BLEUnsignedLongCharacteristic m_syncDataCharacteristic{ BTCOMMON_SYNCDATA_CHARACTERISTIC_UUID, BLERead | BLEWrite };
    BLEUnsignedLongCharacteristic m_timestampCharacteristic{ BTCOMMON_TIMESTAMP_CHARACTERISTIC_UUID, BLERead };
};

#endif