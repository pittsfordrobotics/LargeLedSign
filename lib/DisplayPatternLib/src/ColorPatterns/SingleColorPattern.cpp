#include "SingleColorPattern.h"

SingleColorPattern::SingleColorPattern(uint32_t color)
{
    m_color = color;
    m_colorCount = 1;
}

void SingleColorPattern::reset()
{
}

uint32_t SingleColorPattern::getNextColor()
{
    return m_color;
}

uint16_t SingleColorPattern::getNumberOfParameters()
{
    return getParameterNames().size();
}

void SingleColorPattern::incrementOnly(uint16_t incrementAmount)
{
}

std::vector<String> SingleColorPattern::getParameterNames()
{
    return std::vector<String>();
}