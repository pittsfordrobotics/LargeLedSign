#ifndef ROTATION_DISPLAY_PATTERN_H
#define ROTATION_DISPLAY_PATTERN_H

#include "DisplayPattern.h"

class RotationDisplayPattern : public DisplayPattern
{
    public:
        RotationDisplayPattern(bool isClockwise, bool isSpotLight);

        static std::vector<String> getParameterNames();

        void setNumberOfRays(byte unscaledValue);
        void setAngleIncrementDeg(byte unscaledValue);

    protected:
        virtual void resetInternal(PixelMap* pixelMap);
        virtual void updateInternal(PixelMap* pixelMap);
        virtual uint16_t getMaxUpdateTimeMillis() {return 120;};

    private:
        bool isAngleInRange(float angleToCheck, float startAngle, float endAngle);
        float normalizeAngle(float angle);

        bool m_isClockwise{true};
        bool m_isSpotLight{false};
        uint16_t m_numberOfRays{1};
        int16_t m_angleIncrementDeg{5};
        float m_centerRow{0.0f};
        float m_centerColumn{0.0f};
        float m_currentAngleDeg{0.0f};
        std::vector<std::vector<float>> m_pixelAnglesDeg;
        std::vector<uint32_t> m_spotLightColors;
};

#endif
