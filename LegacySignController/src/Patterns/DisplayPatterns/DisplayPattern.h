#ifndef DISPLAYPATTERNS_H
#define DISPLAYPATTERNS_H

#include <Arduino.h>
#include "Patterns\ColorPatterns\ColorPattern.h"
#include <MathUtils.h>
#include <PixelMap.h>
#include <vector>

class DisplayPattern {
    public:
        // Instantiates a new DisplayPattern.
        DisplayPattern();
        virtual ~DisplayPattern();

        // The ColorPattern passed in here will be "owned" by the DisplayPattern
        // When the DisplayPattern is deleted, the ColorPattern will be deleted as well.
        void setColorPattern(ColorPattern* colorPattern);
        void setSpeed(byte speed);

        // Return value indicates whether an update was performed.
        bool update(PixelMap* pixelMap);
        void reset(PixelMap* pixelMap);

    protected:
        // Called by the "update" method when it's time to update the display.
        virtual void updateInternal(PixelMap* pixelMap) = 0;

        // Called by the "reset" method to perform any pattern-specific reset logic.
        virtual void resetInternal(PixelMap* pixelMap) = 0;
        
        ColorPattern* m_colorPattern;

    private:
        uint32_t m_nextUpdate{0};
        uint16_t m_iterationDelay{0};
};

#endif