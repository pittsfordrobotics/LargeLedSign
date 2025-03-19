#ifndef FIREDISPLAYPATTERN_H
#define FIREDISPLAYPATTERN_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "DisplayPattern.h"
#include "PixelBuffer.h"
#include <MathUtils.h>
#include <vector>

class FireDisplayPattern : public DisplayPattern
{
    public:
        FireDisplayPattern(PixelBuffer* pixelBuffer, bool useIndividualDigits = false);
        void setSparkingAmount(byte sparkingAmount);
        void setCoolingAmount(byte coolingAmount);

        static std::vector<String> getParameterNames();
    
    protected:
        virtual void updateInternal();
        virtual void resetInternal();

    private:
        bool m_useIndividualDigits{false};
        byte m_sparking{120};
        byte m_cooling{50};
        ulong m_heatColors[256];
        
        // This will initialized to the number of rows in the pixel buffer.
        // It maintains the current "heat" level for each row.
        std::vector<int> m_rowHeats;
        std::vector<std::vector<int>> m_rowHeatsPerDigit;

        // ICK
        // List of digits, each of which contains a list of rows,
        // each of which contains the set of pixels in the row.
        std::vector<std::vector<std::vector<int>*>> m_rowsForDigits;

        void setRowHeatColor(int row, byte heat);
        void updateRowHeats(std::vector<int> &rowHeats);
        void GeneratePallet();
};

#endif