#include "SingleColorPattern.h"

SingleColorPattern::SingleColorPattern(ulong color)
{
    m_color = color;
}

void SingleColorPattern::reset()
{
}

ulong SingleColorPattern::getNextColor()
{
    return m_color;
}