#ifndef CENTEROUTDISPLAYPATTERN_H
#define CENTEROUTDISPLAYPATTERN_H

#include <Arduino.h>
#include "DisplayPattern.h"
#include "PixelBuffer.h"
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
        virtual void updateInternal(PixelBuffer* pixelBuffer);
        virtual void resetInternal(PixelBuffer* pixelBuffer);

    private:
        CenterOutOrientation m_orientation{CenterOutOrientation::Horizontal};
        uint m_centerLine;
};

#endif