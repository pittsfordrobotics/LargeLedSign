#include "RainbowColorPattern.h"

RainbowColorPattern::RainbowColorPattern()
{
}

void RainbowColorPattern::reset()
{
    m_currentHue = 0;
}

unsigned long RainbowColorPattern::getNextColor()
{
    unsigned long color = NeoPixelUtils::ColorHSV(m_currentHue);
    m_currentHue += m_hueIncrement;
    return color;
}

unsigned int RainbowColorPattern::getNumberOfParameters()
{
    return getParameterNames().size();
}

void RainbowColorPattern::setHueIncrement(byte hueIncrement)
{
    m_hueIncrement = MathUtils::rescaleInput(5, 1000, hueIncrement);
}

void RainbowColorPattern::incrementOnly(unsigned int incrementAmount)
{
    m_currentHue += incrementAmount * m_hueIncrement;
}

std::vector<String> RainbowColorPattern::getParameterNames()
{
    std::vector<String> parameterNames;
    parameterNames.push_back("Hue increment");

    return parameterNames;
}