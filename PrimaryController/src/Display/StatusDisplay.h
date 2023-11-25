#ifndef STATUSDISPLAY_H
#define STATUSDISPLAY_H

#include <Arduino.h>
#include <TM1637Display.h>
#include <vector>
#include <queue>

enum class DisplayPriority 
{
    None = 1,            // No status should be shown.
    Ephemeral,           // Temporary display, clears automatically after a short time.
    Sequence,            // Display battery levels.
    AdHoc                // Display status until cleared.
};

class StatusDisplay {
    public:
        // Brightnes is 0-7
        StatusDisplay(byte clockPin, byte dioPin, byte brightness);
        ~StatusDisplay();

        // Sets the display to the given string.
        // The display will stay until another display command overrides it.
        void setDisplay(String stringToDisplay);

        // Display the given string for the given amount of time.
        // After this time, the display will be cleared.
        void displayTemporary(String stringToDisplay, uint durationMsec);

        void displaySequence(std::vector<String> stringsToDisplay, uint durationMsec);

        // Updates the display.
        void update();
        void clear();

    private:
        TM1637Display* m_display;
        byte convertCharacter(char c);
        void displayString(String s);

        ulong m_nextUpdate{0};
        uint m_sequenceDuration{0};
        DisplayPriority m_currentPriority{DisplayPriority::None};
        std::queue<String> m_displayQueue;
};

#endif