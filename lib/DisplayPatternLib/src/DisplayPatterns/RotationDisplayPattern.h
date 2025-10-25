#ifndef ROTATION_DISPLAY_PATTERN_H
#define ROTATION_DISPLAY_PATTERN_H

#include "DisplayPattern.h"

class RotationDisplayPattern : public DisplayPattern
{
    public:
        RotationDisplayPattern(boolean isClockwise, PixelBuffer *pixels);

        static std::vector<String> getParameterNames();

        void setNumberOfRays(byte unscaledValue);
        void setAngleIncrementDeg(byte unscaledValue);

    protected:
        virtual void resetInternal();
        virtual void updateInternal();

    private:
        boolean isAngleInRange(float angleToCheck, float startAngle, float endAngle);

        boolean m_isClockwise{true};
        uint m_numberOfRays{1};
        int m_angleIncrementDeg{5};
        float m_centerRow{0.0f};
        float m_centerColumn{0.0f};
        float m_currentAngleDeg{0.0f};
        std::vector<std::vector<float>> m_pixelAnglesDeg;
};

#endif
