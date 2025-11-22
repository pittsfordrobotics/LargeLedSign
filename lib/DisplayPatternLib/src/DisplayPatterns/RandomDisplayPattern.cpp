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

void RandomDisplayPattern::resetInternal(PixelMap* pixelMap)
{
    m_colorPattern->reset();
    // Reset the background to black
    // TBD:
    // Reset the background to a certain color (or rainbow) based on parameters.
    pixelMap->fill(0);
}

void RandomDisplayPattern::updateInternal()
{
    // Grab the next color and fill all needed random pixels with that color.
    // TBD: have the option to grab the next color for each pixel to be filled.
    ulong newColor = m_colorPattern->getNextColor();
    int numPixelsToFill = m_pixelBuffer->getPixelCount() * m_percentToFill;
    m_pixelBuffer->fillRandomly(newColor, numPixelsToFill);
}

void RandomDisplayPattern::updateInternal(PixelMap* pixelMap)
{
    // Grab the next color and fill all needed random pixels with that color.
    // TBD: have the option to grab the next color for each pixel to be filled.
    ulong newColor = m_colorPattern->getNextColor();
    int numPixelsToFill = pixelMap->getPixelCount() * m_percentToFill;
    pixelMap->fillRandomly(newColor, numPixelsToFill);
}

std::vector<String> RandomDisplayPattern::getParameterNames()
{
    std::vector<String> parameterNames;
    parameterNames.push_back("Update amount");

    return parameterNames;
}