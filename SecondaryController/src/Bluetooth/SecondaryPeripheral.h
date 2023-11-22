#ifndef SECONDARYPERIPHERAL_H
#define SECONDARYPERIPHERAL_H

#include <Arduino.h>
#include <ArduinoBLE.h>
#include "BluetoothCommon.h"
#include "CommonPeripheral.h"

class SecondaryPeripheral : public CommonPeripheral {
  public:
    SecondaryPeripheral();
    String getSignConfigurationData();
    void setSignConfigurationData(String signData);
    ulong getSyncData();
    void setSyncData(ulong syncData);

  private:
    String m_currentSignData;
    ulong m_currentSyncData{0};
    BLEStringCharacteristic m_signDataCharacteristic{ BTCOMMON_SIGNDATACHARACTERISTIC_UUID, BLERead | BLEWrite | BLENotify, BTCOMMON_MAXSTRINGLENGTH };
    BLEUnsignedLongCharacteristic m_syncDataCharacteristic{ BTCOMMON_SYNCDATA_CHARACTERISTIC_UUID, BLERead | BLEWrite | BLENotify };
};

#endif