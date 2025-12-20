#ifndef SIGNSTATUS_H
#define SIGNSTATUS_H

#include <Arduino.h>
#include <vector>
#include "SignConfigurationData.h"
#include <PatternData.h>

struct SignStatus {
    public:
        String styleNames;
        String patternNames;
        byte brightness{0};
        byte speed{0};
        SignConfigurationData signConfigurationData;
        PatternData patternData;

        SignStatus();
        SignStatus(const SignStatus& other);
        SignStatus& operator=(const SignStatus& other);
        bool operator==(const SignStatus& other);
        bool operator!=(const SignStatus& other);

    private:
        void copy(const SignStatus& other);
};

#endif