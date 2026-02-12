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
    // Rescale based on "frame rate".
    // Slowest speed will be hard-coded to 0.5 frames per sec,
    // max speed will be the fastest the display pattern can handle.
    int maxMSecPerFrame = getMaxUpdateTimeMillis() + MAX_NEOPIXEL_UPDATE_TIME_MILLIS;
    int maxFrameRateX1000 = (double) 1 / maxMSecPerFrame * 1000 * 1000;
    int minFrameRateX1000 = 500;  // 1 frame every 2 seconds
    int scaledFrameRateX1000 = MathUtils::rescaleInput(minFrameRateX1000, maxFrameRateX1000, speed);

    // Convert scaled frame rate back to millisecond iteration delay between frames.
    m_iterationDelay = (1000 * 1000) / scaledFrameRateX1000;
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