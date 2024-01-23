#ifndef PREDEFINEDSTYLELIST_H
#define PREDEFINEDSTYLELIST_H

#include <Arduino.h>
#include <vector>
#include "PredefinedStyle.h"
#include "PredefinedStyleLib.h"

class PredefinedStyleList {
    public:
        // The default constructor.
        PredefinedStyleList(uint numberOfLists);

        void addStyleToList(uint listNumber, PredefinedStyles style);

        // Gets the style associated with the given button number and sequence number.
        // The sequence number will "wrap around" if needed.
        // If there are no styles associated with the button number, the default style will be returned.
        PredefinedStyle getStyleForButton(uint buttonNumber, uint sequenceNumber);

    private:
        PredefinedStyles m_defaultStyle{ PredefinedStyles::Pink_Solid };
        std::vector<std::vector<PredefinedStyles>> m_buttonStyles;
};

#endif