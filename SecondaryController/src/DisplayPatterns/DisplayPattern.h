#ifndef DISPLAYPATTERNS_H
#define DISPLAYPATTERNS_H

#include <Arduino.h>
#include "../ColorPatterns/ColorPattern.h"
#include "../Math/MathUtils.h"
#include "PixelBuffer.h"

class DisplayPattern {
    public:
        // Instantiates a new DisplayPattern.
        // The PixelBuffer will survive destruction of the class,
        DisplayPattern(PixelBuffer* pixelBuffer);
        ~DisplayPattern();

        // The ColorPattern passed in here will be "owned" by the DisplayPattern
        // When the DisplayPattern is deleted, the ColorPattern will be deleted as well.
        void setColorPattern(ColorPattern* colorPattern);
        void setSpeed(byte speed);

        void update();
        void reset();

    protected:
        // Called by the "update" method when it's time to update the display.
        virtual void updateInternal() = 0;

        // Called by the "reset" method to perform any pattern-specific reset logic.
        virtual void resetInternal() = 0;
        
        ColorPattern* m_colorPattern;
        PixelBuffer* m_pixelBuffer;

    private:
        ulong m_nextUpdate{0};
        uint m_iterationDelay{0};
};

#endif