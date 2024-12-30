#ifndef DISPLAYPATTERNS_H
#define DISPLAYPATTERNS_H

#include <Arduino.h>
#include "ColorPatterns\ColorPattern.h"
#include <MathUtils.h>
#include <PixelBufferLib.h>
#include <vector>
#include <map>

class DisplayPattern {
    public:
        // Instantiates a new DisplayPattern.
        DisplayPattern();
        virtual ~DisplayPattern();

        // The ColorPattern passed in here will be "owned" by the DisplayPattern
        // When the DisplayPattern is deleted, the ColorPattern will be deleted as well.
        void setColorPattern(ColorPattern* colorPattern);
        void setSpeed(byte speed);

        bool update(PixelBuffer* pixelBuffer);
        void reset(PixelBuffer* pixelBuffer);

    protected:
        // Called by the "update" method when it's time to update the display.
        virtual void updateInternal(PixelBuffer* pixelBuffer) = 0;

        // Called by the "reset" method to perform any pattern-specific reset logic.
        virtual void resetInternal(PixelBuffer* pixelBuffer) = 0;
        
        ColorPattern* m_colorPattern;

    private:
        uint m_iterationDelay{0};
        std::map<PixelBuffer*, ulong> m_lastUpdateTimes{};
};

#endif