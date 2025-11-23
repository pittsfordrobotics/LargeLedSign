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

void DisplayPattern::reset(PixelMap* pixelMap)
{
    if (m_colorPattern)
    {
        resetInternal(pixelMap);
    }
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