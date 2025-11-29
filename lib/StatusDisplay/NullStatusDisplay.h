#ifndef NULLSTATUSDISPLAY_H
#define NULLSTATUSDISPLAY_H

#include <Arduino.h>
#include "StatusDisplay.h"

class NullStatusDisplay : public StatusDisplay {
    public:
        NullStatusDisplay() : StatusDisplay() {}

        void setDisplay(String stringToDisplay) override {Serial.println(stringToDisplay);}

        void displayTemporary(String stringToDisplay, uint durationMsec) override {Serial.println(stringToDisplay);}

        void displaySequence(std::vector<String> stringsToDisplay, uint durationMsec) override {}

        void update() override {}

        void clear() override {}
};

#endif