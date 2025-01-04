#include "RandomDisplayPattern.h"

RandomDisplayPattern::RandomDisplayPattern() : DisplayPattern()
{
}

void RandomDisplayPattern::setUpdateAmount(byte updateAmount)
{
    m_percentToFill = MathUtils::rescaleInput(1, 50, updateAmount) / 100.0;
}

void RandomDisplayPattern::resetInternal(PixelBuffer* pixelBuffer)
{
    m_colorPattern->reset();
    // Reset the background to black
    // TBD:
    // Reset the background to a certain color (or rainbow) based on parameters.
    pixelBuffer->fill(0);
}

void RandomDisplayPattern::updateInternal(PixelBuffer* pixelBuffer)
{
    // Grab the next color and fill all needed random pixels with that color.
    // TBD: have the option to grab the next color for each pixel to be filled.
    unsigned long newColor = m_colorPattern->getNextColor();
    int numPixelsToFill = pixelBuffer->getPixelCount() * m_percentToFill;
    pixelBuffer->fillRandomly(newColor, numPixelsToFill);
}

std::vector<String> RandomDisplayPattern::getParameterNames()
{
    std::vector<String> parameterNames;
    parameterNames.push_back("Update amount");

    return parameterNames;
}