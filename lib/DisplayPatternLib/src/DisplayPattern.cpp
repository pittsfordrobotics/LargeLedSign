#include "DisplayPattern.h"

DisplayPattern::DisplayPattern()
{
    m_pixelBuffer = nullptr;
}

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

// Legacy method using PixelBuffer.
void DisplayPattern::reset()
{
    if (m_colorPattern && m_pixelBuffer)
    {
        resetInternal();
    }
}

void DisplayPattern::reset(PixelMap* pixelMap)
{
    if (m_colorPattern)
    {
        resetInternal(pixelMap);
    }
}

// Legacy method using PixelBuffer.
void DisplayPattern::update()
{
    if (!m_colorPattern || !m_pixelBuffer)
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

bool DisplayPattern::update(PixelMap* pixelMap)
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
    updateInternal(pixelMap);
    return true;
}   