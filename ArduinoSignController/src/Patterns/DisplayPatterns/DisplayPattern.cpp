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

    int consecutiveUpdates = 0;
    while (m_nextUpdate <= millis())
    {
        if (consecutiveUpdates > 0)
        {
            m_droppedFrames++;
        }

        if (consecutiveUpdates >= 3)
        {
            // Prevent potential infinite loop if updates are taking too long.
            m_nextUpdate = millis() + m_iterationDelay;
            break;
        }

        m_nextUpdate += m_iterationDelay;
        uint32_t startTime = millis();
        updateInternal(pixelMap);
        m_totalTimeUpdatingPixelMap += (millis() - startTime);
        m_numberOfPixelMapUpdates++;
        consecutiveUpdates++;
    }

    return true;
}

uint16_t DisplayPattern::getDroppedFrames()
{
    return m_droppedFrames;
}

double DisplayPattern::getAverageTimeUpdatingPixelMap()
{
    if (m_totalTimeUpdatingPixelMap == 0)
    {
        return 0;
    }

    return (double) m_totalTimeUpdatingPixelMap / m_numberOfPixelMapUpdates;
}

void DisplayPattern::resetTelemetry()
{
    m_droppedFrames = 0;
    m_numberOfPixelMapUpdates = 0;
    m_totalTimeUpdatingPixelMap = 0;
}