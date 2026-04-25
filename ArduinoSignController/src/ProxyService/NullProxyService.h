#include "IProxyService.h"

/*
 A "null" implementation of the IProxyService interface that does nothing.
*/
class NullProxyService : public IProxyService
{
    public:
        void setStatusDisplay(StatusDisplay* display) override {}
        void disconnectSecondaries() override {}
        void connectToSecondaries() override {}
        void update() override {}
        void setConfiguration(byte brightness, byte speed, PatternData pattern) override {}
        void displaySecondaryBatteryVoltages() override {}
        byte getCurrentBrightness() override { return 170; } // Some reasonable default brightness value.
};