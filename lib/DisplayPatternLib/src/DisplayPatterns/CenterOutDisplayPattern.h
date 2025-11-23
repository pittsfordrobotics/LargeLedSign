#ifndef CENTEROUTDISPLAYPATTERN_H
#define CENTEROUTDISPLAYPATTERN_H

#include <Arduino.h>
#include "DisplayPattern.h"
#include <vector>
#include <algorithm>

enum class CenterOutOrientation {
    Horizontal,
    Vertical
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
        uint16_t m_centerLine;
};

#endif