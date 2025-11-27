#ifndef CENTEROUTDISPLAYPATTERN_H
#define CENTEROUTDISPLAYPATTERN_H

#include <Arduino.h>
#include "DisplayPattern.h"
#include <vector>
#include <algorithm>

enum class CenterOutOrientation {
    Horizontal,
    Vertical,
    Radial
};

class CenterOutDisplayPattern : public DisplayPattern {
    public:
        CenterOutDisplayPattern(CenterOutOrientation orientation);

        static std::vector<String> getParameterNames();

    protected:
        virtual void updateInternal(PixelMap* pixelMap);
        virtual void resetInternal(PixelMap* pixelMap);

    private:
        CenterOutOrientation m_orientation{CenterOutOrientation::Horizontal};
        int m_centerRow;
        int m_centerColumn;

        void updateHorizontal(PixelMap* pixelMap, uint32_t newColor);
        void updateVertical(PixelMap* pixelMap, uint32_t newColor);
        void updateRadial(PixelMap* pixelMap, uint32_t newColor);
};

#endif