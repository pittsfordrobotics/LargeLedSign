#ifndef BLEPROXYSERVICE_H
#define BLEPROXYSERVICE_H

#define MAX_SECONDARY_SCAN_TIME 2000
#define MAX_TOTAL_SCAN_TIME 10000
#define MAX_SECONDARY_COUNT 5
#define SECONDARY_PING_INTERVAL 3000

#include <algorithm>
#include "IProxyService.h"
#include "Bluetooth/SecondaryClient.h"
#include "StatusDisplay/NullStatusDisplay.h"
#include "Utils/DebugUtils.h"

class BleProxyService : public IProxyService
{
    public:
        BleProxyService();
        ~BleProxyService();
        void update() override;
        void setStatusDisplay(StatusDisplay* display) override;
        void disconnectSecondaries() override;
        void connectToSecondaries() override;
        void setConfiguration(byte brightness, byte speed, PatternData pattern) override;
        void displaySecondaryBatteryVoltages() override;
        byte getCurrentBrightness() override;

    private:
        void populateSecondaryClients();
        void updateOffsetDataForSecondaryClients();
        void checkSecondaryConnections();
        SecondaryClient *scanForSecondaryClient();

        std::vector<SecondaryClient *> allSecondaries;
        StatusDisplay* statusDisplay;
        StatusDisplay* nullStatusDisplay;

        byte currentBrightness;
        byte currentSpeed;
        PatternData currentPatternData;
        unsigned long nextSecondaryConnectionCheck;
};

#endif // BLEPROXYSERVICE_H