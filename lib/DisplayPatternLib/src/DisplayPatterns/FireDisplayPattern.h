#ifndef FIREDISPLAYPATTERN_H
#define FIREDISPLAYPATTERN_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "DisplayPattern.h"
#include "PixelBuffer.h"
#include <MathUtils.h>
#include <vector>

enum class FirePatternType : byte
{
    Solid = 0,
    Digit = 1,
    IndividualRows = 2
};

class FireDisplayPattern : public DisplayPattern
{
    public:
        FireDisplayPattern();
        FireDisplayPattern(PixelBuffer* pixelBuffer, FirePatternType patternType);
        void setSparkingAmount(byte sparkingAmount);
        void setCoolingAmount(byte coolingAmount);

        static std::vector<String> getParameterNames();
    
    protected:
        virtual void updateInternal();
        virtual void updateInternal(PixelMap* pixelMap);
        virtual void resetInternal();
        virtual void resetInternal(PixelMap* pixelMap);

    private:
        FirePatternType m_patternType{FirePatternType::Solid};
        byte m_sparking{120};
        byte m_cooling{50};
        ulong m_heatColors[256];
        
        // Maintains the current "heat" level for each row grouping.
        std::vector<std::vector<int>> m_combinedRowHeats;

        // List of row groups (ex: digits), each of which contains a list of rows,
        // each of which contains the set of pixels in the row.
        std::vector<std::vector<std::vector<int>>> m_combinedRowGroups;

        void setRowHeatColor(int row, byte heat);
        void updateRowHeats(std::vector<int> &rowHeats);
        void generatePallet();
        void populateAllRows();
        void populateCombinedGroupsForDigits();
        void populateCombinedGroupsForIndividualColumns();
};

#endif