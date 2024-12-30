#include "DisplayPattern.h"

DisplayPattern::DisplayPattern(PixelBuffer* pixelBuffer)
{
    m_pixelBuffer = pixelBuffer;
}

DisplayPattern::~DisplayPattern()
{
    if (m_colorPattern)
    {
        delete m_colorPattern;
    }
}

void DisplayPattern::setColorPattern(ColorPattern* colorPattern)
{
    m_colorPattern = colorPattern;
}

void DisplayPattern::setSpeed(byte speed)
{
    m_iterationDelay = MathUtils::rescaleInput(500, 5, speed);
}

void DisplayPattern::reset(PixelBuffer* pixelBuffer)
{
    if (m_colorPattern)
    {
        resetInternal(pixelBuffer);
    }
}

bool DisplayPattern::update(PixelBuffer* pixelBuffer)
{
    if (!m_colorPattern)
    {
        return false;
    }
    
    if (millis() < m_nextUpdate)
    {
        return false;
    }

    m_nextUpdate = millis() + m_iterationDelay;
    updateInternal(pixelBuffer);
    
    return true;
}
