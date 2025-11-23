#ifndef SIMPLESHIFTDISPLAYPATTERN_H
#define SIMPLESHIFTDISPLAYPATTERN_H

#include <Arduino.h>
#include "DisplayPattern.h"

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
        SimpleShiftDisplayPattern(ShiftType shiftType);

        static std::vector<String> getParameterNames();

    protected:
        virtual void updateInternal(PixelMap* pixelMap);
        virtual void resetInternal(PixelMap* pixelMap);

    private:
        int getNumberOfBlocksForPattern(PixelMap* pixelMap);
        int getInitialIncrementAmount(PixelMap* pixelMap);
        ShiftType m_shiftType{ShiftType::Right};
};

#endif