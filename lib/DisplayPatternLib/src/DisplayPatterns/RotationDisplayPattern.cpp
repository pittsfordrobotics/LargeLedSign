#include "RotationDisplayPattern.h"

RotationDisplayPattern::RotationDisplayPattern(boolean isClockwise, PixelBuffer *pixelBuffer) : DisplayPattern(pixelBuffer), m_isClockwise(isClockwise)
{
    m_isClockwise = isClockwise;
}

void RotationDisplayPattern::setNumberOfRays(byte unscaledValue)
{
    int numberOfRays = MathUtils::rescaleInput(1, 5, unscaledValue);
    if (numberOfRays > 4)
    {
        numberOfRays = 4;
    }

    m_numberOfRays = numberOfRays;
}

void RotationDisplayPattern::setAngleIncrementDeg(byte unscaledValue)
{
    m_angleIncrementDeg = MathUtils::rescaleInput(1, 15, unscaledValue);
}

void RotationDisplayPattern::resetInternal()
{
    m_colorPattern->reset();

    // Calculate center point
    // TODO: take into account offsets for columns to left/right
    m_centerRow = m_pixelBuffer->getRowCount() / 2.0f;
    m_centerColumn = m_pixelBuffer->getColumnCount() / 2.0f;
}

void RotationDisplayPattern::updateInternal()
{
    // Hard code 1 degree per update for now.
    // Only 1 "ray" for now.

    // Due to the fact that the pixel buffer's row and column numbers start
    // at 0 in the top-left, an increasing angle is clockwise.
    float newAngleDeg = m_isClockwise ? m_currentAngleDeg + m_angleIncrementDeg : m_currentAngleDeg - m_angleIncrementDeg;
    ulong currentColor = m_colorPattern->getNextColor();

    for (int row = 0; row < m_pixelBuffer->getRowCount(); row++)
    {
        for (int col = 0; col < m_pixelBuffer->getColumnCount(); col++)
        {
            // Calculate angle from center to this pixel.
            float deltaY = row - m_centerRow;
            float deltaX = col - m_centerColumn;
            float angleToPixelDeg = std::atan2(deltaY, deltaX) * (180.0f / M_PI);
            if (angleToPixelDeg < 0.0f)
            {
                angleToPixelDeg += 360.0f;
            }

            if (isAngleInRange(angleToPixelDeg, m_currentAngleDeg, newAngleDeg))
            {
                m_pixelBuffer->setColorInPixelMap(row, col, currentColor);
            }
        }
    }

    m_currentAngleDeg = newAngleDeg;
    if (m_currentAngleDeg >= 360.0f)
    {
        m_currentAngleDeg -= 360.0f;
    }
    if (m_currentAngleDeg < 0.0f)
    {
        m_currentAngleDeg += 360.0f;
    }
}

boolean RotationDisplayPattern::isAngleInRange(float angleToCheck, float startAngle, float endAngle)
{
    if (startAngle < endAngle)
    {
        // angle is increasing
        if (angleToCheck >= startAngle && angleToCheck < endAngle)
        {
            return true;
        }
        // Check if we've wrapped around past 360 degrees
        if (endAngle >= 360.0f)
        {
            if (angleToCheck + 360 >= startAngle && angleToCheck + 360 < endAngle)
            {
                return true;
            }
        }
    }
    else
    {
        // angle is decreasing
        if (angleToCheck <= startAngle && angleToCheck > endAngle)
        {
            return true;
        }
        // Check if we've wrapped around past 0 degrees
        if (endAngle < 0.0f)
        {
            if (angleToCheck - 360 <= startAngle && angleToCheck - 360 > endAngle)
            {
                return true;
            }
        }
    }

    return false;
}

std::vector<String> RotationDisplayPattern::getParameterNames()
{
    std::vector<String> parameterNames;

    parameterNames.push_back("Number of rays");
    parameterNames.push_back("Angle increment");

    return parameterNames;
}