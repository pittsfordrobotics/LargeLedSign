#ifndef DISPLAYPATTERNS_H
#define DISPLAYPATTERNS_H

#include <Arduino.h>
#include "ColorPatterns\ColorPattern.h"
#include <MathUtils.h>
#include <PixelBufferLib.h>
#include <PixelMap.h>
#include <vector>

class DisplayPattern {
    public:
        // Instantiates a new DisplayPattern.
        // The PixelBuffer will survive destruction of the class.
        // Legacy constructor until the patterns move to using PixelMap.
        DisplayPattern(PixelBuffer* pixelBuffer);
        DisplayPattern();
        virtual ~DisplayPattern();

        // The ColorPattern passed in here will be "owned" by the DisplayPattern
        // When the DisplayPattern is deleted, the ColorPattern will be deleted as well.
        void setColorPattern(ColorPattern* colorPattern);
        void setSpeed(byte speed);

        // Legacy update method using PixelBuffer.
        void update();
        // Legacy reset method using PixelBuffer.
        void reset();

        // Return value indicates whether an update was performed.
        bool update(PixelMap* pixelMap);
        void reset(PixelMap* pixelMap);

    protected:
        // Called by the "update" method when it's time to update the display.
        virtual void updateInternal() = 0;
        virtual void updateInternal(PixelMap* pixelMap) = 0;

        // Called by the "reset" method to perform any pattern-specific reset logic.
        virtual void resetInternal() = 0;
        virtual void resetInternal(PixelMap* pixelMap) = 0;
        
        ColorPattern* m_colorPattern;
        PixelBuffer* m_pixelBuffer;

    private:
        ulong m_nextUpdate{0};
        uint m_iterationDelay{0};
};

#endif