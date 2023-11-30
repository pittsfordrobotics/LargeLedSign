#include "Arduino.h"
#include "LightStyle.h"
#include "PixelBuffer.h"

std::vector<String> LightStyle::knownPatterns{ LIGHTPATTERN_NAMES };

LightStyle::LightStyle(String name, PixelBuffer *pixelBuffer)
{
    m_pixelBuffer = pixelBuffer;
    m_name = name;
}

void LightStyle::setSpeed(uint8_t speed)
{
    m_speed = speed;
}

void LightStyle::setStep(byte step)
{
    m_step = step;
}

String LightStyle::getName()
{
    return m_name;
}

void LightStyle::setPattern(byte pattern)
{
    m_pattern = pattern;
}

int LightStyle::getNumberOfBlocksForPattern()
{
    switch (m_pattern)
    {
    case static_cast<int>(LightPatterns::Right):
    case static_cast<int>(LightPatterns::Left):
        return m_pixelBuffer->getColumnCount();
    case static_cast<int>(LightPatterns::Up):
    case static_cast<int>(LightPatterns::Down):
        return m_pixelBuffer->getRowCount();
    default: // Solid, digit, or undfined
        // All lights in the sign use the same color
        return 1;
    }
}

int LightStyle::getNumberOfBlocksToDrain()
{
    switch (m_pattern)
    {
    case static_cast<int>(LightPatterns::Right):
        return m_pixelBuffer->getColsToRight();
    case static_cast<int>(LightPatterns::Left):
        return m_pixelBuffer->getColsToLeft();
    case static_cast<int>(LightPatterns::Digit):
        return m_pixelBuffer->getDigitsToRight();
    default: // Solid or anything else
        return 0;
    }
}

void LightStyle::shiftColorUsingPattern(uint32_t newColor)
{
    switch (m_pattern)
    {
        case static_cast<int>(LightPatterns::Right):
            m_pixelBuffer->shiftColumnsRight(newColor);
            return;
        case static_cast<int>(LightPatterns::Left):
            m_pixelBuffer->shiftColumnsLeft(newColor);
            return;
        case static_cast<int>(LightPatterns::Up):
            m_pixelBuffer->shiftRowsUp(newColor);
            return;
        case static_cast<int>(LightPatterns::Down):
            m_pixelBuffer->shiftRowsDown(newColor);
            return;
        default:
            // Default to Solid (ie, all lights the same color)
            for (uint i = 0; i < m_pixelBuffer->getPixelCount(); i++)
            {
                m_pixelBuffer->setPixel(i, newColor);
            }
    }
}
