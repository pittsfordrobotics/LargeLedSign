#ifndef RADIAL_H
#define RADIAL_H

#include <Arduino.h>
#include <vector>
#include <cmath>
#include "DisplayPattern.h"

class RadialDisplayPattern : public DisplayPattern {
    public:
        RadialDisplayPattern();

    protected:
        virtual void updateInternal(PixelMap* pixelMap) override;
        virtual void resetInternal(PixelMap* pixelMap) override;

    private:
        float m_centerRow{0.0f};
        float m_centerColumn{0.0f};
        std::vector<uint32_t> m_radialColors;
        std::vector<std::vector<float>> m_pixelDistances;
};

#endif