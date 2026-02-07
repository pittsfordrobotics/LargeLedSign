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

        // Maps the byte value of "temperature" to an actual RGB color value.
        uint32_t m_heatColors[256];
        
        // Maintains the current "heat" level for the rows in each column.
        // m_heatMap[column][row] = heat_value
        std::vector<std::vector<int>> m_heatMap;

        void updateRowHeats(std::vector<int> &rowHeats);
        void generatePallet();
};

#endif