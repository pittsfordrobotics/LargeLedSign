#ifndef STYLELIST_H
#define STYLELIST_H

#include <Arduino.h>
#include <vector>
#include "StyleDefinition.h"
#include "StyleDefinitionBuilder.h"
#include "Colors.h"

class StyleList {
    public:
        // The default constructor.
        StyleList(uint numberOfLists);

        void addStyleToList(uint listNumber, const StyleDefinition& style);

        // Gets the style associated with the given list and sequence number.
        // The sequence number will "wrap around" if needed.
        // If there are no styles associated with the list, the default style will be returned.
        StyleDefinition getStyle(uint listNumber, uint sequenceNumber);

    private:
        std::vector<std::vector<StyleDefinition>> m_styleLists;
};

#endif