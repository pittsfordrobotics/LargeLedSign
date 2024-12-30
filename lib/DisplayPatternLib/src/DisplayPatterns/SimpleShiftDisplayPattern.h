#ifndef SIMPLESHIFTDISPLAYPATTERN_H
#define SIMPLESHIFTDISPLAYPATTERN_H

#include <Arduino.h>
#include "DisplayPattern.h"
#include "PixelBuffer.h"

enum class ShiftType {
    Right,
    Left,
    Up,
    Down,
    Digit,
    Line
};

class SimpleShiftDisplayPattern : public DisplayPattern{
    public:
        SimpleShiftDisplayPattern(ShiftType shiftType, PixelBuffer* pixelBuffer);

        static std::vector<String> getParameterNames();

    protected:
        virtual void updateInternal(PixelBuffer* pixelBuffer);
        virtual void resetInternal(PixelBuffer* pixelBuffer);

    private:
        int getNumberOfBlocksForPattern(PixelBuffer* pixelBuffer);
        int getInitialIncrementAmount(PixelBuffer* pixelBuffer);
        ShiftType m_shiftType{ShiftType::Right};
};

#endif