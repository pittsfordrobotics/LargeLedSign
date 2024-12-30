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
        virtual void updateInternal();
        virtual void updateInternal(PixelBuffer2* pixelBuffer);
        virtual void resetInternal();
        virtual void resetInternal(PixelBuffer2* pixelBuffer);

    private:
        int getNumberOfBlocksForPattern();
        int getNumberOfBlocksForPattern(PixelBuffer2* pixelBuffer);
        int getInitialIncrementAmount();
        int getInitialIncrementAmount(PixelBuffer2* pixelBuffer);
        ShiftType m_shiftType{ShiftType::Right};
};

#endif