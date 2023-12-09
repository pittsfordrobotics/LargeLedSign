#include "RandomDisplayPattern.h"

RandomDisplayPattern::RandomDisplayPattern(PixelBuffer* pixelBuffer) : DisplayPattern(pixelBuffer)
{
}

void RandomDisplayPattern::setUpdateAmount(byte updateAmount)
{
    m_percentToFill = MathUtils::rescaleInput(1, 50, updateAmount) / 100.0;
}

void RandomDisplayPattern::resetInternal()
{
    m_colorPattern->reset();
    // Reset the background to black
    // TBD:
    // Reset the background to a certain color (or rainbow) based on parameters.
    m_pixelBuffer->fill(0);
}

void RandomDisplayPattern::updateInternal()
{
    // Grab the next color and fill all needed random pixels with that color.
    // TBD: have the option to grab the next color for each pixel to be filled.
    ulong newColor = m_colorPattern->getNextColor();
    int numPixelsToFill = m_pixelBuffer->getPixelCount() * m_percentToFill;
    m_pixelBuffer->fillRandomly(newColor, numPixelsToFill);
}