#ifndef STYLELIST_H
#define STYLELIST_H

#include <Arduino.h>
#include <vector>
#include "StyleDefinition.h"
#include "CommonStyles.h"
#include "Colors.h"

class StyleList {
    public:
        // The default constructor.
        StyleList(unsigned int numberOfLists);

        void addStyleToList(unsigned int listNumber, const StyleDefinition& style);

        // Gets the style associated with the given list and sequence number.
        // The sequence number will "wrap around" if needed.
        // If there are no styles associated with the list, the default style will be returned.
        StyleDefinition getStyle(unsigned int listNumber, unsigned int sequenceNumber);

    private:
        std::vector<std::vector<StyleDefinition>> m_styleLists;
};

#endif