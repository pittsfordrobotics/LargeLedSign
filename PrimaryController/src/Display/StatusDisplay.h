#ifndef STATUSDISPLAY_H
#define STATUSDISPLAY_H

#include <Arduino.h>
#include <TM1637Display.h>

enum class DisplayPriority 
{
    None = 1,            // No status should be shown.
    ConnectionIndicator, // Connected to Central.
    BatteryLevel,        // Display battery levels.
    Ephimeral,           // Temporary display, clears automatically after a short time.
    AdHoc                // Display status until cleared.
};

class StatusDisplay {
    public:
        // Brightnes is 0-7
        StatusDisplay(byte clockPin, byte dioPin, byte brightness);
        ~StatusDisplay();

        void adhocDisplay(String stringToDisplay);
        void update();
        void clear();

    private:
        TM1637Display* m_display;
        void loadStringToBuffer(String s);
        byte convertCharacter(char c);

        byte m_displayBuffer[4];
};

#endif