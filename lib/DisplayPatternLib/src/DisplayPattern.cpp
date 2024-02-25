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

void DisplayPattern::reset()
{
    if (m_colorPattern)
    {
        resetInternal();
    }
}

void DisplayPattern::update()
{
    if (!m_colorPattern)
    {
        return;
    }
    
    if (millis() < m_nextUpdate)
    {
        return;
    }

    m_nextUpdate = millis() + m_iterationDelay;
    updateInternal();
}