#include "RotationDisplayPattern.h"

RotationDisplayPattern::RotationDisplayPattern(boolean isClockwise, boolean isSpotLight, PixelBuffer *pixelBuffer) : DisplayPattern(pixelBuffer)
{
    m_isClockwise = isClockwise;
    m_isSpotLight = isSpotLight;
}

void RotationDisplayPattern::setNumberOfRays(byte unscaledValue)
{
    int numberOfRays = MathUtils::rescaleInput(1, 7, unscaledValue);
    if (numberOfRays > 6)
    {
        numberOfRays = 6;
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
    m_spotLightColors.clear();

    for (int i = 0; i < m_numberOfRays; i++)
    {
        m_spotLightColors.push_back(m_colorPattern->getNextColor());
    }

    // Calculate center point
    // TODO: take into account offsets for columns to left/right
    m_centerRow = m_pixelBuffer->getRowCount() / 2.0f;
    m_centerColumn = m_pixelBuffer->getColumnCount() / 2.0f;

    // Calculate the angle from the center to each pixel.
    for (int row = 0; row < m_pixelBuffer->getRowCount(); row++)
    {
        std::vector<float> anglesInRow;
        for (int col = 0; col < m_pixelBuffer->getColumnCount(); col++)
        {
            float deltaY = row - m_centerRow;
            float deltaX = col - m_centerColumn;
            float angleToPixelDeg = std::atan2(deltaY, deltaX) * (180.0f / M_PI);
            if (angleToPixelDeg < 0.0f)
            {
                angleToPixelDeg += 360.0f;
            }

            anglesInRow.push_back(angleToPixelDeg);
        }
        
        m_pixelAnglesDeg.push_back(anglesInRow);
    }
}

void RotationDisplayPattern::updateInternal()
{
    // Due to the fact that the pixel buffer's row and column numbers start
    // at 0 in the top-left, an increasing angle is clockwise.
    float newAngleDeg = m_isClockwise ? m_currentAngleDeg + m_angleIncrementDeg : m_currentAngleDeg - m_angleIncrementDeg;
    ulong currentColor = m_colorPattern->getNextColor();

    for (int row = 0; row < m_pixelBuffer->getRowCount(); row++)
    {
        for (int col = 0; col < m_pixelBuffer->getColumnCount(); col++)
        {
            float angleToPixelDeg = m_pixelAnglesDeg[row][col];
            float rayAngleIncrement = 360.0f / m_numberOfRays;
            for (uint rayIndex = 0; rayIndex < m_numberOfRays; rayIndex++)
            {
                float adjustedStartAngle = m_currentAngleDeg + rayIndex * rayAngleIncrement;
                float adjustedEndAngle = newAngleDeg + rayIndex * rayAngleIncrement;

                if (isAngleInRange(angleToPixelDeg, adjustedStartAngle, adjustedEndAngle))
                {
                    if (m_isSpotLight)
                    {
                        m_pixelBuffer->setColorInPixelMap(row, col, m_spotLightColors[rayIndex]);
                    }
                    else
                    {
                        m_pixelBuffer->setColorInPixelMap(row, col, currentColor);
                    }
                }
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

// This method assumes that endAngle = startAngle +/- some increment.
// It assumes that no angle-wrapping has been applied to the start or end angles.
boolean RotationDisplayPattern::isAngleInRange(float angleToCheck, float startAngle, float endAngle)
{
    float normalizedStartAngle = normalizeAngle(startAngle);
    float normalizedEndAngle = normalizeAngle(endAngle);
    float normalizedAngleToCheck = normalizeAngle(angleToCheck);

    if (startAngle < endAngle)
    {
        // angle is increasing
        if (normalizedStartAngle <= normalizedEndAngle) {
            return normalizedAngleToCheck >= normalizedStartAngle && normalizedAngleToCheck <= normalizedEndAngle;
        }
        // Wraparound case
        return normalizedAngleToCheck >= normalizedStartAngle || normalizedAngleToCheck <= normalizedEndAngle;
    }
    else
    {
        // angle is decreasing
        if (normalizedEndAngle <= normalizedStartAngle) {
            return normalizedAngleToCheck >= normalizedEndAngle && normalizedAngleToCheck <= normalizedStartAngle;
        }
        // Wraparound case
        return normalizedAngleToCheck >= normalizedEndAngle || normalizedAngleToCheck <= normalizedStartAngle;
    }
}

float RotationDisplayPattern::normalizeAngle(float angle)
{
    angle = fmod(angle, 360.0);
    if (angle < 0) {
        angle += 360.0;
    }

    return angle;
}

std::vector<String> RotationDisplayPattern::getParameterNames()
{
    std::vector<String> parameterNames;

    parameterNames.push_back("Number of rays");
    parameterNames.push_back("Angle increment");

    return parameterNames;
}