#include "LowPowerDisplayPattern.h"

LowPowerDisplayPattern::LowPowerDisplayPattern(PixelBuffer* pixelBuffer) : DisplayPattern(pixelBuffer)
{
}

void LowPowerDisplayPattern::resetInternal()
{
    m_iterationCount = 0;
    m_colorPattern->reset();
    
    // Fill the buffer with black
    m_pixelBuffer->fill(0);
}

void LowPowerDisplayPattern::resetInternal(PixelMap* pixelMap)
{
    m_iterationCount = 0;
    m_colorPattern->reset();
    
    // Fill the buffer with black
    pixelMap->fill(0);
}

void LowPowerDisplayPattern::updateInternal()
{
    // We're just emitting a single blinking pixel.
    // Instead of calling m_colorPattern->getNextColor() each time,
    // alternate between setting all to black and setting the first pixel
    // to the "next color" from the pattern (which should normally be just a solid color).
    if (m_iterationCount % 2 == 0)
    {
        m_pixelBuffer->setPixel(0, m_colorPattern->getNextColor());
    }
    else
    {
        m_pixelBuffer->setPixel(0, 0);
    }
    
    m_iterationCount++;    
}

void LowPowerDisplayPattern::updateInternal(PixelMap* pixelMap)
{
    // We're just emitting a single blinking pixel.
    // Instead of calling m_colorPattern->getNextColor() each time,
    // alternate between setting all to black and setting the first pixel
    // to the "next color" from the pattern (which should normally be just a solid color).
    if (m_iterationCount % 2 == 0)
    {
        pixelMap->setPixel(0, m_colorPattern->getNextColor());
    }
    else
    {
        pixelMap->setPixel(0, 0);
    }
    
    m_iterationCount++;    
}

std::vector<String> LowPowerDisplayPattern::getParameterNames()
{
    return std::vector<String>();
}