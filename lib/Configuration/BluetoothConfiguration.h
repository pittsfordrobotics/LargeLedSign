#ifndef BLUETOOTHCONFIG_H
#define BLUETOOTHCONFIG_H

#include <Arduino.h>
#include <ArduinoJson.h>

class BluetoothConfiguration
{
    public:
        BluetoothConfiguration();
        BluetoothConfiguration(const BluetoothConfiguration& other);
        BluetoothConfiguration(bool isEnabled, String uuid, String localName);
        
        bool isEnabled() { return m_enabled; }
        String getUuid() { return m_uuid; }
        String getLocalName() { return m_localName; }

        BluetoothConfiguration& operator=(const BluetoothConfiguration& other);

    private:
        void copy(const BluetoothConfiguration& other);

        bool m_enabled{true};
        String m_uuid{"99be4fac-c708-41e5-a149-74047f554cc1"};
        String m_localName{"LED Sign Controller"};
};

#endif