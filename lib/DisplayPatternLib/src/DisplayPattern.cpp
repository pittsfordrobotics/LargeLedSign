#include "DisplayPattern.h"

DisplayPattern::DisplayPattern()
{
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
    
    // Find the last time this buffer was updated.
    if (m_lastUpdateTimes.find(pixelBuffer) == m_lastUpdateTimes.end())
    {
        // Not found. Add the initial value of 0.
        m_lastUpdateTimes[pixelBuffer] = 0;
    }

    ulong nextUpdate = m_lastUpdateTimes[pixelBuffer];

    if (millis() < m_nextUpdate)
    {
        return false;
    }

    m_lastUpdateTimes[pixelBuffer] = millis() + m_iterationDelay;
    updateInternal(pixelBuffer);
    
    return true;
}
