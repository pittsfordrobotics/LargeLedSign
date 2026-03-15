#include <Arduino.h>
#include "Configuration/BatteryMonitorConfiguration.h"

class BatteryMonitor
{
    public:
        BatteryMonitor(const BatteryMonitorConfiguration& configuration);

        float getCalculatedBatteryVoltage();
        int getRawVoltageInputLevel();
        bool isEnabled() { return m_config.isEnabled(); }
        bool isBatteryBelowLowPowerThreshold();
        bool isBatteryAboveRestartThreshold();

    private:
        BatteryMonitorConfiguration m_config;
};