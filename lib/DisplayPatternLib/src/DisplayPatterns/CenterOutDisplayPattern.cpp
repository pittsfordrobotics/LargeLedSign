#include "CenterOutDisplayPattern.h"

CenterOutDisplayPattern::CenterOutDisplayPattern(CenterOutOrientation orientation, PixelBuffer* pixelBuffer) : DisplayPattern(pixelBuffer)
{
    m_orientation = orientation;
}

std::vector<String> CenterOutDisplayPattern::getParameterNames()
{
    return std::vector<String>();
}

void CenterOutDisplayPattern::resetInternal()
{
    m_colorPattern->reset();
    if (m_orientation == CenterOutOrientation::Vertical)
    {
        m_centerLine = m_pixelBuffer->getRowCount() / 2 - 1;

        int rowsToFill = m_pixelBuffer->getRowCount() - m_centerLine;
        for (int i = 0; i < rowsToFill; i++)
        {
            updateInternal();
        }
    }
    else
    {
        m_centerLine = (m_pixelBuffer->getColsToLeft() + m_pixelBuffer->getColsToRight() + m_pixelBuffer->getColumnCount()) / 2 - 1;

        if (m_centerLine > m_pixelBuffer->getColsToLeft() + m_pixelBuffer->getColumnCount())
        {
            // Center line is to the right of this display, so we'll only be shifting left.
            m_colorPattern->incrementOnly(m_pixelBuffer->getColsToLeft());
            int colsToFill = m_pixelBuffer->getColumnCount();
            for (int i = 0; i < colsToFill; i++)
            {
                updateInternal();
            }
        }
        else if (m_centerLine < m_pixelBuffer->getColsToLeft())
        {
            // Center line is to the left of this display, so we'll only be shifting right.
            m_colorPattern->incrementOnly(m_pixelBuffer->getColsToRight());
            int colsToFill = m_pixelBuffer->getColumnCount();
            for (int i = 0; i < colsToFill; i++)
            {
                updateInternal();
            }
        }
        else
        {
            // Center line is inside this display.
            int shiftAmount = std::min(m_pixelBuffer->getColsToLeft(), m_pixelBuffer->getColsToRight());
            m_colorPattern->incrementOnly(shiftAmount);
            
            int amountToFillLeft = m_centerLine + 1 - m_pixelBuffer->getColsToLeft();
            int amountToFillRight = m_pixelBuffer->getColsToLeft() + m_pixelBuffer->getColumnCount() - (m_centerLine + 1);
            int amountToFill = std::max(amountToFillLeft, amountToFillRight);
            
            for (int i = 0; i < amountToFill; i++)
            {
                updateInternal();
            }
        }
    }    
}

void CenterOutDisplayPattern::updateInternal()
{
    ulong newColor = m_colorPattern->getNextColor();

    if (m_orientation == CenterOutOrientation::Vertical)
    {
        m_pixelBuffer->shiftRowsUp(newColor, m_centerLine);
        m_pixelBuffer->shiftRowsDown(newColor, m_centerLine + 1);
    }
    else
    {
        if (m_centerLine > m_pixelBuffer->getColsToLeft() + m_pixelBuffer->getColumnCount())
        {
            // Center line is to the right of this display, so we'll only be shifting left.
            m_pixelBuffer->shiftColumnsLeft(newColor);
        }
        else if (m_centerLine < m_pixelBuffer->getColsToLeft())
        {
            // Center line is to the left of this display, so we'll only be shifting right.
            m_pixelBuffer->shiftColumnsRight(newColor);
        }
        else
        {
            uint localCenter = m_centerLine - m_pixelBuffer->getColsToLeft();
            m_pixelBuffer->shiftColumnsLeft(newColor, localCenter);
            if (localCenter < m_pixelBuffer->getColumnCount() - 1) {
                m_pixelBuffer->shiftColumnsRight(newColor, localCenter + 1);
            }
        }
    }
}

void CenterOutDisplayPattern::resetInternal(PixelMap* pixelMap)
{
    m_colorPattern->reset();
    if (m_orientation == CenterOutOrientation::Vertical)
    {
        m_centerLine = pixelMap->getRowCount() / 2 - 1;

        int rowsToFill = pixelMap->getRowCount() - m_centerLine;
        for (int i = 0; i < rowsToFill; i++)
        {
            updateInternal(pixelMap);
        }
    }
    else
    {
        m_centerLine = (pixelMap->getColsToLeft() + pixelMap->getColsToRight() + pixelMap->getColumnCount()) / 2 - 1;

        if (m_centerLine > pixelMap->getColsToLeft() + pixelMap->getColumnCount())
        {
            // Center line is to the right of this display, so we'll only be shifting left.
            m_colorPattern->incrementOnly(pixelMap->getColsToLeft());
            int colsToFill = pixelMap->getColumnCount();
            for (int i = 0; i < colsToFill; i++)
            {
                updateInternal(pixelMap);
            }
        }
        else if (m_centerLine < pixelMap->getColsToLeft())
        {
            // Center line is to the left of this display, so we'll only be shifting right.
            m_colorPattern->incrementOnly(pixelMap->getColsToRight());
            int colsToFill = pixelMap->getColumnCount();
            for (int i = 0; i < colsToFill; i++)
            {
                updateInternal(pixelMap);
            }
        }
        else
        {
            // Center line is inside this display.
            int shiftAmount = std::min(pixelMap->getColsToLeft(), pixelMap->getColsToRight());
            m_colorPattern->incrementOnly(shiftAmount);
            
            int amountToFillLeft = m_centerLine + 1 - pixelMap->getColsToLeft();
            int amountToFillRight = pixelMap->getColsToLeft() + pixelMap->getColumnCount() - (m_centerLine + 1);
            int amountToFill = std::max(amountToFillLeft, amountToFillRight);
            
            for (int i = 0; i < amountToFill; i++)
            {
                updateInternal(pixelMap);
            }
        }
    }    
}

void CenterOutDisplayPattern::updateInternal(PixelMap* pixelMap)
{
    ulong newColor = m_colorPattern->getNextColor();

    if (m_orientation == CenterOutOrientation::Vertical)
    {
        pixelMap->shiftRowsUp(newColor, m_centerLine);
        pixelMap->shiftRowsDown(newColor, m_centerLine + 1);
    }
    else
    {
        if (m_centerLine > pixelMap->getColsToLeft() + pixelMap->getColumnCount())
        {
            // Center line is to the right of this display, so we'll only be shifting left.
            pixelMap->shiftColumnsLeft(newColor);
        }
        else if (m_centerLine < pixelMap->getColsToLeft())
        {
            // Center line is to the left of this display, so we'll only be shifting right.
            pixelMap->shiftColumnsRight(newColor);
        }
        else
        {
            uint localCenter = m_centerLine - pixelMap->getColsToLeft();
            pixelMap->shiftColumnsLeft(newColor, localCenter);
            if (localCenter < pixelMap->getColumnCount() - 1) {
                pixelMap->shiftColumnsRight(newColor, localCenter + 1);
            }
        }
    }
}
