#ifndef SECONDARYCLIENT_H
#define SECONDARYCLIENT_H

#include <Arduino.h>
#include <ArduinoBLE.h>
#include <BluetoothCommon.h>
#include "StringUtils.h"
#include "ServiceStatus.h"

class SecondaryClient {
    public:
        SecondaryClient(BLEDevice peripheral);
        ~SecondaryClient();

        bool isConnected();
        void disconnect();
        bool isValidClient() { return m_isValid; }
        String getLocalName();

        // SignOrder is part of ServiceStatus, but it's used when
        // initially sorting the clients. Expose it directly here
        // so we don't end up hitting the BT service to get the status
        // every time. It shouldn't ever change, so caching it here is fine.
        int getSignOrder() { return m_signOrder; }

        String getStringValue(String characteristicUuid);
        byte getByteValue(String characteristicUuid);
        ServiceStatus getServiceStatus();

    private:
        BLEDevice m_peripheral;
        bool m_isValid;
        int m_signOrder;

        void initialize();
};

#endif