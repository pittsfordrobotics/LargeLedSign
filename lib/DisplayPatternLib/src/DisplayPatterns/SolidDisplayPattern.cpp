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
    m_pixelBuffer->fill(m_colorPattern->getNextColor());
}

void SolidDisplayPattern::resetInternal(PixelBuffer* pixelBuffer)
{
    m_colorPattern->reset();
    pixelBuffer->fill(m_colorPattern->getNextColor());
}

void SolidDisplayPattern::updateInternal(PixelBuffer* pixelBuffer)
{
    pixelBuffer->fill(m_colorPattern->getNextColor());
}

std::vector<String> SolidDisplayPattern::getParameterNames()
{
    return std::vector<String>();
}