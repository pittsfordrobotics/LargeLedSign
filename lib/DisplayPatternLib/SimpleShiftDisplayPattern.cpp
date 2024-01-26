#include "SimpleShiftDisplayPattern.h"

SimpleShiftDisplayPattern::SimpleShiftDisplayPattern(ShiftType shiftType, PixelBuffer* pixelBuffer) : DisplayPattern(pixelBuffer)
{
    m_shiftType = shiftType;
}

void SimpleShiftDisplayPattern::resetInternal()
{
    m_colorPattern->reset();
    m_colorPattern->incrementOnly(getInitialIncrementAmount());

    int numberOfBlocks = getNumberOfBlocksForPattern();
    for (int i = 0; i < numberOfBlocks; i++)
    {
        updateInternal();
    }
}

void SimpleShiftDisplayPattern::updateInternal()
{
    ulong newColor = m_colorPattern->getNextColor();

    switch (m_shiftType)
    {
        case ShiftType::Right:
            m_pixelBuffer->shiftColumnsRight(newColor);
            return;
        case ShiftType::Left:
            m_pixelBuffer->shiftColumnsLeft(newColor);
            return;
        case ShiftType::Up:
            m_pixelBuffer->shiftRowsUp(newColor);
            return;
        case ShiftType::Down:
            m_pixelBuffer->shiftRowsDown(newColor);
            return;
        case ShiftType::Digit:
            m_pixelBuffer->shiftDigitsRight(newColor);
            return;
        case ShiftType::Line:
            m_pixelBuffer->shiftLine(newColor);
            return;
        default:
            // Default to Solid (ie, all lights the same color)
            m_pixelBuffer->fill(newColor);
    }
}

int SimpleShiftDisplayPattern::getInitialIncrementAmount()
{
    switch (m_shiftType)
    {
        case ShiftType::Right:
            return m_pixelBuffer->getColsToRight();
        case ShiftType::Left:
            return m_pixelBuffer->getColsToLeft();
        case ShiftType::Digit:
            return m_pixelBuffer->getDigitsToRight();
        default:
            return 0;
    }
}

int SimpleShiftDisplayPattern::getNumberOfBlocksForPattern()
{
    switch (m_shiftType)
    {
        case ShiftType::Right:
        case ShiftType::Left:
            return m_pixelBuffer->getColumnCount();
        case ShiftType::Up:
        case ShiftType::Down:
            return m_pixelBuffer->getRowCount();
        default:
            // All lights in the sign use the same color
            return 1;
    }
}

std::vector<String> SimpleShiftDisplayPattern::getParameterNames()
{
    return std::vector<String>();
}
