#include "SolidDisplayPattern.h"

SolidDisplayPattern::SolidDisplayPattern(PixelBuffer* pixelBuffer) : DisplayPattern(pixelBuffer)
{
}

void SolidDisplayPattern::resetInternal()
{
    m_colorPattern->reset();
    m_pixelBuffer->fill(m_colorPattern->getNextColor());
}

void SolidDisplayPattern::updateInternal()
{
    // Nothing to do.
}

std::vector<String> SolidDisplayPattern::getParameterNames()
{
    return std::vector<String>();
}