#include "SingleColorPattern.h"

SingleColorPattern::SingleColorPattern(unsigned long color)
{
    m_color = color;
    m_colorCount = 1;
}

void SingleColorPattern::reset()
{
}

unsigned long SingleColorPattern::getNextColor()
{
    return m_color;
}

unsigned int SingleColorPattern::getNumberOfParameters()
{
    return getParameterNames().size();
}

void SingleColorPattern::incrementOnly(unsigned int incrementAmount)
{
}

std::vector<String> SingleColorPattern::getParameterNames()
{
    return std::vector<String>();
}