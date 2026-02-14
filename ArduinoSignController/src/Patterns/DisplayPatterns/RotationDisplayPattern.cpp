#include "RotationDisplayPattern.h"

RotationDisplayPattern::RotationDisplayPattern(bool isClockwise, bool isSpotLight) : DisplayPattern()
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

void RotationDisplayPattern::resetInternal(PixelMap* pixelMap)
{
    m_colorPattern->reset();
    m_spotLightColors.clear();

    for (int i = 0; i < m_numberOfRays; i++)
    {
        m_spotLightColors.push_back(m_colorPattern->getNextColor());
    }

    uint16_t columnCount = pixelMap->getColumnCount();
    uint16_t colsToLeft = pixelMap->getColsToLeft();
    uint16_t colsToRight = pixelMap->getColsToRight();
    uint16_t rowCount = pixelMap->getRowCount();

    // Calculate center point
    m_centerRow = rowCount / 2.0f;
    m_centerColumn = (columnCount + colsToLeft + colsToRight) / 2.0f;

    // Calculate the angle from the center to each pixel.
    for (int row = 0; row < rowCount; row++)
    {
        std::vector<float> anglesInRow;
        for (int col = 0; col < columnCount; col++)
        {
            float deltaY = row - m_centerRow;
            float deltaX = colsToLeft + col - m_centerColumn;
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

void RotationDisplayPattern::updateInternal(PixelMap* pixelMap)
{
    // Cache values that don't change
    const int rowCount = pixelMap->getRowCount();
    const int colCount = pixelMap->getColumnCount();
    const float rayAngleIncrement = 360.0f / m_numberOfRays;
    
    // Due to the fact that the pixel buffer's row and column numbers start
    // at 0 in the top-left, an increasing angle is clockwise.
    float newAngleDeg = m_isClockwise ? m_currentAngleDeg + m_angleIncrementDeg : m_currentAngleDeg - m_angleIncrementDeg;
    uint32_t currentColor = m_colorPattern->getNextColor();

    for (int row = 0; row < rowCount; row++)
    {
        for (int col = 0; col < colCount; col++)
        {
            float angleToPixelDeg = m_pixelAnglesDeg[row][col];
            
            for (uint16_t rayIndex = 0; rayIndex < m_numberOfRays; rayIndex++)
            {
                float adjustedStartAngle = m_currentAngleDeg + rayIndex * rayAngleIncrement;
                float adjustedEndAngle = newAngleDeg + rayIndex * rayAngleIncrement;

                if (isAngleInRange(angleToPixelDeg, adjustedStartAngle, adjustedEndAngle))
                {
                    uint32_t colorToSet = m_isSpotLight ? m_spotLightColors[rayIndex] : currentColor;
                    pixelMap->setColorInPixelMap(row, col, colorToSet);
                    break; // Early exit - pixel found its ray
                }
            }
        }
    }

    // Normalize angle once at the end
    m_currentAngleDeg = newAngleDeg;
    if (m_currentAngleDeg >= 360.0f)
    {
        m_currentAngleDeg -= 360.0f;
    }
    else if (m_currentAngleDeg < 0.0f)
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
    while (angle >= 360.0f) {
        angle -= 360.0f;
    }
    while (angle < 0.0f) {
        angle += 360.0f;
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