#include "RainbowColorPattern.h"

void RainbowColorPattern::reset()
{
    m_currentHue = 0;
}

ulong RainbowColorPattern::getNextColor()
{
    ulong color = Adafruit_NeoPixel::ColorHSV(m_currentHue);
    m_currentHue += m_hueIncrement;
    return color;
}

void RainbowColorPattern::setHueIncrement(byte hueIncrement)
{
    m_hueIncrement = MathUtils::rescaleInput(5, 1000, hueIncrement);
}