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
    Digit
};

class SimpleShiftDisplayPattern : public DisplayPattern{
    public:
        SimpleShiftDisplayPattern(ShiftType shiftType, PixelBuffer* pixelBuffer);

    protected:
        virtual void updateInternal();
        virtual void resetInternal();

    private:
        int getNumberOfBlocksForPattern();
        int getInitialIncrementAmount();
        ShiftType m_shiftType{ShiftType::Right};
};

#endif