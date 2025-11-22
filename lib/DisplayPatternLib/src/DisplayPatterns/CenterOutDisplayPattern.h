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
        CenterOutDisplayPattern();
        CenterOutDisplayPattern(CenterOutOrientation orientation, PixelBuffer* pixelBuffer);

        static std::vector<String> getParameterNames();

    protected:
        virtual void updateInternal();
        virtual void updateInternal(PixelMap* pixelMap);
        virtual void resetInternal();
        virtual void resetInternal(PixelMap* pixelMap);

    private:
        CenterOutOrientation m_orientation{CenterOutOrientation::Horizontal};
        uint m_centerLine;
};

#endif