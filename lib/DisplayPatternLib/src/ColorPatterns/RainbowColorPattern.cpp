#include "RainbowColorPattern.h"

RainbowColorPattern::RainbowColorPattern()
{
}

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

uint16_t RainbowColorPattern::getNumberOfParameters()
{
    return getParameterNames().size();
}

void RainbowColorPattern::setHueIncrement(byte hueIncrement)
{
    m_hueIncrement = MathUtils::rescaleInput(5, 1000, hueIncrement);
}

void RainbowColorPattern::incrementOnly(uint16_t incrementAmount)
{
    m_currentHue += incrementAmount * m_hueIncrement;
}

std::vector<String> RainbowColorPattern::getParameterNames()
{
    std::vector<String> parameterNames;
    parameterNames.push_back("Hue increment");

    return parameterNames;
}