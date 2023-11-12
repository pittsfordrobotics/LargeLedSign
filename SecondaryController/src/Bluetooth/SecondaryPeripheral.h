#ifndef SECONDARYPERIPHERAL_H
#define SECONDARYPERIPHERAL_H

#include <Arduino.h>
#include <ArduinoBLE.h>
#include "BluetoothCommon.h"
#include "CommonPeripheral.h"

class SecondaryPeripheral : public CommonPeripheral {
  public:
    SecondaryPeripheral();
    String getSignData();
    void setSignData(String signData);

  private:
    String m_currentSignData;
    BLEStringCharacteristic m_signDataCharacteristic{ BTCOMMON_SIGNDATACHARACTERISTIC_UUID, BLERead | BLEWrite, BTCOMMON_MAXSTRINGLENGTH };
};

#endif