#ifndef BLUETOOTHCONFIG_H
#define BLUETOOTHCONFIG_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "BluetoothCommon.h"

class BluetoothConfig
{
    public:
        static BluetoothConfig* ParseJson(const char* jsonString, size_t length);

        String getUuid() { return m_uuid; }
        String getLocalName() { return m_localName; }

    private:
        BluetoothConfig();
        String m_uuid{BTCOMMON_PRIMARYCONTROLLER_UUID};
        String m_localName{"LED Sign Controller"};
};

#endif