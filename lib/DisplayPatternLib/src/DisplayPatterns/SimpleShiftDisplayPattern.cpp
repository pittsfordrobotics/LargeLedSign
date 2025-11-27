#include "SimpleShiftDisplayPattern.h"

SimpleShiftDisplayPattern::SimpleShiftDisplayPattern(ShiftType shiftType) : DisplayPattern()
{
    m_shiftType = shiftType;
}

void SimpleShiftDisplayPattern::resetInternal(PixelMap* pixelMap)
{
    m_colorPattern->reset();
    m_colorPattern->incrementOnly(getInitialIncrementAmount(pixelMap));

    int numberOfBlocks = getNumberOfBlocksForPattern(pixelMap);
    for (int i = 0; i < numberOfBlocks; i++)
    {
        updateInternal(pixelMap);
    }
}

void SimpleShiftDisplayPattern::updateInternal(PixelMap* pixelMap)
{
    uint32_t newColor = m_colorPattern->getNextColor();

    switch (m_shiftType)
    {
        case ShiftType::Right:
            pixelMap->shiftColumnsRight();
            pixelMap->setColumnColor(0, newColor);
            return;
        case ShiftType::Left:
            pixelMap->shiftColumnsLeft();
            pixelMap->setColumnColor(pixelMap->getColumnCount() - 1, newColor);
            return;
        case ShiftType::Up:
            pixelMap->shiftRowsUp();
            pixelMap->setRowColor(pixelMap->getRowCount() - 1, newColor);
            return;
        case ShiftType::Down:
            pixelMap->shiftRowsDown();
            pixelMap->setRowColor(0, newColor);
            return;
        case ShiftType::Digit:
            pixelMap->shiftDigitsRight();
            pixelMap->setDigitColor(0, newColor);
            return;
        case ShiftType::Line:
            pixelMap->shiftPixelsRight();
            pixelMap->setRawPixel(0, newColor);
            return;
        default:
            // Default to Solid (ie, all lights the same color)
            pixelMap->fill(newColor);
    }
}

int SimpleShiftDisplayPattern::getInitialIncrementAmount(PixelMap* pixelMap)
{
    switch (m_shiftType)
    {
        case ShiftType::Right:
            return pixelMap->getColsToRight();
        case ShiftType::Left:
            return pixelMap->getColsToLeft();
        case ShiftType::Digit:
            return pixelMap->getDigitsToRight();
        default:
            return 0;
    }
}

int SimpleShiftDisplayPattern::getNumberOfBlocksForPattern(PixelMap* pixelMap)
{
    switch (m_shiftType)
    {
        case ShiftType::Right:
        case ShiftType::Left:
            return pixelMap->getColumnCount();
        case ShiftType::Up:
        case ShiftType::Down:
            return pixelMap->getRowCount();
        default:
            // All lights in the sign use the same color
            return 1;
    }
}

std::vector<String> SimpleShiftDisplayPattern::getParameterNames()
{
    return std::vector<String>();
}
