#ifndef SECONDARYCLIENT_H
#define SECONDARYCLIENT_H

#include <Arduino.h>
#include <ArduinoBLE.h>
#include <BluetoothCommon.h>

class SecondaryClient {
    public:
        SecondaryClient(BLEDevice* peripheral);
        ~SecondaryClient();

        bool isConnected();
        void disconnect();
        String getLocalName();
        uint getSignOrder() { return m_signOrder; }
        uint getColumnCount() { return m_columnCount; }
        uint getPixelCount() { return m_pixelCount; }

    private:
        BLEDevice* m_peripheral;
        uint m_signOrder;
        uint m_columnCount;
        uint m_pixelCount;

        void initialize();
        String getStringValue(BLECharacteristic characteristic);
        std::vector<String> splitString(String input, char delimiter);
};

#endif