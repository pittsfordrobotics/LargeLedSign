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
        CenterOutDisplayPattern(CenterOutOrientation orientation, PixelBuffer* pixelBuffer);

        static std::vector<String> getParameterNames();

    protected:
        virtual void updateInternal();
        virtual void resetInternal();

    private:
        CenterOutOrientation m_orientation{CenterOutOrientation::Horizontal};
        uint m_centerLine;
};

#endif