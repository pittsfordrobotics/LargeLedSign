#ifndef BUTTONCONFIGURATION_H
#define BUTTONCONFIGURATION_H

#include <Arduino.h>
#include <vector>
#include "PredefinedStyle.h"

class ButtonConfiguration {
    public:
        // The default constructor.
        ButtonConfiguration();

        // Gets the style associated with the given button number and sequence number.
        // The sequence number will "wrap around" if needed.
        // If there are no styles associated with the button number, the default style will be returned.
        PredefinedStyle getStyleForButton(uint buttonNumber, uint sequenceNumber);

        // Gets the style associated with the given button number and sequence number.
        // An "alternate" style could be used for long-presses or double-taps.
        // The sequence number will "wrap around" if needed.
        // If there are no styles associated with the button number, the default style will be returned.
        PredefinedStyle getAlternateStyleForButton(uint buttonNumer, uint sequenceNumber);

    private:
        PredefinedStyles m_defaultStyle{ PredefinedStyles::Pink_Solid };
        std::vector<std::vector<PredefinedStyles>> m_buttonStyles;
        std::vector<std::vector<PredefinedStyles>> m_alternateButtonStyles;
};

#endif