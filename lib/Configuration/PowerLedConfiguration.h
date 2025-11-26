#ifndef POWERLEDCONFIGURATION_H
#define POWERLEDCONFIGURATION_H

class PowerLedConfiguration {
    public:
        PowerLedConfiguration();
        PowerLedConfiguration(bool enabled, int gpioPin);
        PowerLedConfiguration(const PowerLedConfiguration& other);
    
        bool isEnabled() { return m_enabled; }
        int getGpioPin() { return m_dataGpioPin; }

        PowerLedConfiguration& operator=(const PowerLedConfiguration& other);

    private:
        void copy(const PowerLedConfiguration& other);

        bool m_enabled{false};
        int m_dataGpioPin{0};
};

#endif