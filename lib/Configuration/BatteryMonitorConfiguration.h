#ifndef BATTERYMONITORCONFIGURATION_H
#define BATTERYMONITORCONFIGURATION_H

class BatteryMonitorConfiguration {
    public:
        BatteryMonitorConfiguration();
        BatteryMonitorConfiguration(bool enabled,
                                    int analogInputPin,
                                    float inputMultiplier,
                                    float voltageToEnterLowPowerState,
                                    float voltageToExitLowPowerState);

        BatteryMonitorConfiguration(const BatteryMonitorConfiguration& other);

        bool isEnabled() { return m_enabled; }
        int getAnalogInputPin() { return m_analogInputPin; }
        float getInputMultiplier() { return m_inputMultiplier; }
        float getVoltageToEnterLowPowerState() { return m_voltageToEnterLowPowerState; }
        float getVoltageToExitLowPowerState() { return m_voltageToExitLowPowerState; }

        BatteryMonitorConfiguration& operator=(const BatteryMonitorConfiguration& other);

    private:
        void copy(const BatteryMonitorConfiguration& other);

        bool m_enabled{false};
        int m_analogInputPin{1};
        float m_inputMultiplier{3.0};
        float m_voltageToEnterLowPowerState{6.5f};
        float m_voltageToExitLowPowerState{8.0f};
};

#endif