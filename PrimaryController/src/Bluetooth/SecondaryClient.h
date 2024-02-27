#ifndef SECONDARYCLIENT_H
#define SECONDARYCLIENT_H

#include <Arduino.h>
#include <ArduinoBLE.h>
#include <BluetoothCommon.h>
#include <SignDataLib.h>
#include <PatternData.h>

/******************************
 * Provides communication functionality to the secondary sign controllers.
*/
class SecondaryClient {
    public:
        SecondaryClient(BLEDevice peripheral);
        ~SecondaryClient();

        // Indicates whether the client is still connected to the remote controller.
        bool isConnected();

        // Disconnects the client session.
        void disconnect();

        // Indicates whether the detected client is valid.
        bool isValidClient() { return m_isValid; }

        // Gets the remote client's name.
        String getLocalName();

        // SignOrder is part of SignStatus, but it's used when
        // initially sorting the clients. Expose it directly here
        // so we don't end up hitting the BT service to get the status
        // every time. It shouldn't ever change, so caching it here is fine.
        int getSignOrder() { return m_signOrder; }
        SignStatus getSignStatus();
        float getBatteryVoltage();
        void setBrightness(byte brightness);
        void setSpeed(byte speed);
        void setSignOffsetData(const SignOffsetData& data);
        void setPatternData(const PatternData& data);
        void updateSyncData(ulong syncData);
        String getColorPatternList();
        String getDisplayPatternList();
        long getTimestamp();

    private:
        BLEDevice m_peripheral;
        bool m_isValid{false};
        int m_signOrder{0};

        void initialize();
        String getStringValue(String characteristicUuid);
        byte getByteValue(String characteristicUuid);
        float getFloatValue(String characteristicUuid);
        long getLongValue(String characeristicUuid);
};

#endif