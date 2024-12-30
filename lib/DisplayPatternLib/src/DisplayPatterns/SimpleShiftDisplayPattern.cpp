#include "SimpleShiftDisplayPattern.h"

SimpleShiftDisplayPattern::SimpleShiftDisplayPattern(ShiftType shiftType, PixelBuffer* pixelBuffer) : DisplayPattern(pixelBuffer)
{
    m_shiftType = shiftType;
}

void SimpleShiftDisplayPattern::resetInternal(PixelBuffer* pixelBuffer)
{
    m_colorPattern->reset();
    m_colorPattern->incrementOnly(getInitialIncrementAmount(pixelBuffer));

    int numberOfBlocks = getNumberOfBlocksForPattern(pixelBuffer);
    for (int i = 0; i < numberOfBlocks; i++)
    {
        updateInternal(pixelBuffer);
    }
}

void SimpleShiftDisplayPattern::updateInternal(PixelBuffer* pixelBuffer)
{
    ulong newColor = m_colorPattern->getNextColor();

    switch (m_shiftType)
    {
        case ShiftType::Right:
            pixelBuffer->shiftColumnsRight(newColor);
            return;
        case ShiftType::Left:
            pixelBuffer->shiftColumnsLeft(newColor);
            return;
        case ShiftType::Up:
            pixelBuffer->shiftRowsUp(newColor);
            return;
        case ShiftType::Down:
            pixelBuffer->shiftRowsDown(newColor);
            return;
        case ShiftType::Digit:
            pixelBuffer->shiftDigitsRight(newColor);
            return;
        case ShiftType::Line:
            pixelBuffer->shiftLine(newColor);
            return;
        default:
            // Default to Solid (ie, all lights the same color)
            pixelBuffer->fill(newColor);
    }
}

int SimpleShiftDisplayPattern::getInitialIncrementAmount(PixelBuffer* pixelBuffer)
{
    switch (m_shiftType)
    {
        case ShiftType::Right:
            return pixelBuffer->getColsToRight();
        case ShiftType::Left:
            return pixelBuffer->getColsToLeft();
        case ShiftType::Digit:
            return pixelBuffer->getDigitsToRight();
        default:
            return 0;
    }
}

int SimpleShiftDisplayPattern::getNumberOfBlocksForPattern(PixelBuffer* pixelBuffer)
{
    switch (m_shiftType)
    {
        case ShiftType::Right:
        case ShiftType::Left:
            return pixelBuffer->getColumnCount();
        case ShiftType::Up:
        case ShiftType::Down:
            return pixelBuffer->getRowCount();
        default:
            // All lights in the sign use the same color
            return 1;
    }
}

std::vector<String> SimpleShiftDisplayPattern::getParameterNames()
{
    return std::vector<String>();
}
