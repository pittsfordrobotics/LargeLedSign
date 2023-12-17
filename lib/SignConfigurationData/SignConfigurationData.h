#ifndef SIGNCONFIGURATIONDATA_H
#define SIGNCONFIGURATIONDATA_H

#include <Arduino.h>

struct SignConfigurationData {
    public:
        byte signType{0};
        byte signOrder{0};
        byte digitCount{0};
        byte columnCount{0};
        ushort pixelCount{0};

        // Creates a new SignConfigurationData struct with default values.
        SignConfigurationData();

        // Creates a new SignConfigurationData struct with values copied from the provided SignConfigurationData.
        SignConfigurationData(const SignConfigurationData& other);

        // Sets the values of the current SignConfigurationData to be the same as the provided SignConfigurationData.
        SignConfigurationData& operator=(const SignConfigurationData& other);

        // Returns true if this SignConfigurationData is the same as the provided SignConfigurationData.
        bool operator==(const SignConfigurationData& other);

        // Returns true if this SignConfigurationData is different than the provided SignConfigurationData.
        bool operator!=(const SignConfigurationData& other);

    private:
        void copy(const SignConfigurationData& other);
};

#endif