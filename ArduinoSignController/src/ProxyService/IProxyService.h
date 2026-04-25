#ifndef IPROXYSERVICE_H
#define IPROXYSERVICE_H

#include <Arduino.h>
#include "Patterns/PatternData.h"
#include "StatusDisplay/StatusDisplay.h"

class IProxyService
{
    public:
        virtual void setStatusDisplay(StatusDisplay* display) = 0;
        virtual void disconnectSecondaries() = 0;
        virtual void connectToSecondaries() = 0;
        virtual void update() = 0;
        virtual void setConfiguration(byte brightness, byte speed, PatternData pattern) = 0;
        virtual void displaySecondaryBatteryVoltages() = 0;
        virtual byte getCurrentBrightness() = 0;
};

#endif // IPROXYSERVICE_H
