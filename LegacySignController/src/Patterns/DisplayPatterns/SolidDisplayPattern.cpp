#include "SolidDisplayPattern.h"

SolidDisplayPattern::SolidDisplayPattern() : DisplayPattern()
{
}

void SolidDisplayPattern::resetInternal(PixelMap* pixelMap)
{
    m_colorPattern->reset();
    pixelMap->fill(m_colorPattern->getNextColor());
}

void SolidDisplayPattern::updateInternal(PixelMap* pixelMap)
{
    pixelMap->fill(m_colorPattern->getNextColor());
}

std::vector<String> SolidDisplayPattern::getParameterNames()
{
    return std::vector<String>();
}