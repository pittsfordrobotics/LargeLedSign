#ifndef SECONDARYCLIENT_H
#define SECONDARYCLIENT_H

#include <Arduino.h>
#include <ArduinoBLE.h>
#include <BluetoothCommon.h>
#include <StringUtils.h>
#include <SignStatus.h>
#include <PatternData.h>

class SecondaryClient {
    public:
        SecondaryClient(BLEDevice peripheral);
        ~SecondaryClient();

        bool isConnected();
        void disconnect();
        bool isValidClient() { return m_isValid; }
        String getLocalName();

        // SignOrder is part of SignStatus, but it's used when
        // initially sorting the clients. Expose it directly here
        // so we don't end up hitting the BT service to get the status
        // every time. It shouldn't ever change, so caching it here is fine.
        int getSignOrder() { return m_signOrder; }
        SignStatus getSignStatus();
        float getBatteryVoltage();
        void setBrightness(byte brightness);
        void setPattern(byte pattern);
        void setStyle(byte style);
        void setSpeed(byte speed);
        void setStep(byte step);
        void setSignOffsetData(String offsetData);
        void setPatternData(const PatternData& data);
        void updateSyncData(ulong syncData);

    private:
        BLEDevice m_peripheral;
        bool m_isValid;
        int m_signOrder;

        void initialize();
        String getStringValue(String characteristicUuid);
        byte getByteValue(String characteristicUuid);
        float getFloatValue(String characteristicUuid);
};

#endif