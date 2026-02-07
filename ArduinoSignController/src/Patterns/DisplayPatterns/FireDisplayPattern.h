#ifndef FIREDISPLAYPATTERN_H
#define FIREDISPLAYPATTERN_H

#include <Arduino.h>
#include "Utils\NeoPixelUtils.h"
#include "DisplayPattern.h"
#include "Utils\MathUtils.h"
#include <vector>

class FireDisplayPattern : public DisplayPattern
{
    public:
        FireDisplayPattern();
        void setSparkingAmount(byte sparkingAmount);
        void setCoolingAmount(byte coolingAmount);

        static std::vector<String> getParameterNames();
    
    protected:
        virtual void updateInternal(PixelMap* pixelMap);
        virtual void resetInternal(PixelMap* pixelMap);

    private:
        byte m_sparking{120};
        byte m_cooling{50};
        uint32_t m_heatColors[256];
        
        // Maintains the current "heat" level for the rows in each column.
        // m_rowHeats[column][row] = heat_value
        std::vector<std::vector<int>> m_rowHeats;

        // List of row groups (ex: digits), each of which contains a list of rows,
        // each of which contains the set of pixels in the row.
        std::vector<std::vector<std::vector<int>>> m_combinedRowGroups;

        void updateRowHeats(std::vector<int> &rowHeats);
        void generatePallet();
        void populateCombinedGroupsForIndividualColumns(PixelMap* pixelMap);
};

#endif