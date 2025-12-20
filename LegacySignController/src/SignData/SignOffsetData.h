#ifndef SIGNOFFSETDATA_H
#define SIGNOFFSETDATA_H

#include <Arduino.h>

struct SignOffsetData {
    public:
        byte digitsToRight{0};
        byte digitsToLeft{0};
        ushort columnsToLeft{0};
        ushort columnsToRight{0};

        // Creates a new SignOffsetData struct with default values.
        SignOffsetData();

        // Creates a new SignOffsetData struct with values copied from the provided SignOffsetData.
        SignOffsetData(const SignOffsetData& other);

        // Sets the values of the current SignOffsetData to be the same as the provided SignOffsetData.
        SignOffsetData& operator=(const SignOffsetData& other);

        // Returns true if this SignOffsetData is the same as the provided SignOffsetData.
        bool operator==(const SignOffsetData& other);

        // Returns true if this SignOffsetData is different than the provided SignOffsetData.
        bool operator!=(const SignOffsetData& other);

    private:
        void copy(const SignOffsetData& other);
};

#endif