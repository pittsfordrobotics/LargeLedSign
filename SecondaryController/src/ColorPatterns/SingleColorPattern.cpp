#include "SingleColorPattern.h"

SingleColorPattern::SingleColorPattern(ulong color)
{
    m_color = color;
    m_colorCount = 1;
}

void SingleColorPattern::reset()
{
}

ulong SingleColorPattern::getNextColor()
{
    return m_color;
}

void SingleColorPattern::incrementOnly(uint incrementAmount)
{
}