#ifndef STATUSDISPLAY_H
#define STATUSDISPLAY_H

#include <Arduino.h>
#include <vector>

enum class DisplayPriority 
{
    None = 1,            // No status should be shown.
    Ephemeral,           // Temporary display, clears automatically after a short time.
    Sequence,            // Display battery levels.
    AdHoc                // Display status until cleared.
};

class StatusDisplay {
    public:
        // Sets the display to the given string.
        // The display will stay until another "setDisplay" or "clear" command overrides it.
        // The string to display can be up to 4 characters, plus any dots. Extra characters will be ignored.
        // Example: "A=1.2"
        // A dot in the string will be applied to the prior segment.
        // To display just a dot with an empty digit, use a space before the dot.
        // Example: to display "....", the string should be " . . . ."
        // Any characters that are not recognized will be displayed as a space.
        virtual void setDisplay(String stringToDisplay) = 0;

        // Display the given string for the given amount of time.
        // After this time, the display will be cleared.
        virtual void displayTemporary(String stringToDisplay, uint durationMsec) = 0;

        // Display a sequence of strings, automatically displaying the next
        // string in the sequence after the given period of time.
        // When all strings have been displayed, the display will be cleared.
        virtual void displaySequence(std::vector<String> stringsToDisplay, uint durationMsec) = 0;

        // Updates the display.
        virtual void update() = 0;

        // Clears the display.
        virtual void clear() = 0;
};

#endif