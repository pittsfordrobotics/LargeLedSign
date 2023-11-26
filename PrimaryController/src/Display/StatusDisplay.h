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
        // Initializes the TM1637 quad 7-segment display.
        // The clockPin and dioPin are the digital pin #s that control the TM1637.
        // Brightness should be a value between 0 (off) and 7 (max).
        StatusDisplay(byte clockPin, byte dioPin, byte brightness);
        ~StatusDisplay();

        // Sets the display to the given string.
        // The display will stay until another "setDisplay" or "clear" command overrides it.
        // The string to display can be up to 4 characters, plus any dots. Extra characters will be ignored.
        // Example: "A=1.2"
        // A dot in the string will be applied to the prior segment.
        // To display just a dot with an empty digit, use a space before the dot.
        // Example: to display "....", the string should be " . . . ."
        // Any characters that are not recognized will be displayed as a space.
        void setDisplay(String stringToDisplay);

        // Display the given string for the given amount of time.
        // After this time, the display will be cleared.
        void displayTemporary(String stringToDisplay, uint durationMsec);

        // Display a sequence of strings, automatically displaying the next
        // string in the sequence after the given period of time.
        // When all strings have been displayed, the display will be cleared.
        void displaySequence(std::vector<String> stringsToDisplay, uint durationMsec);

        // Updates the display.
        void update();

        // Clears the display.
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