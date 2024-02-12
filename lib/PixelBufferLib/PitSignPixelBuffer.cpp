#include "PitSignPixelBuffer.h"

PitSignPixelBuffer::PitSignPixelBuffer(int gpioPin) : PixelBuffer(gpioPin)
{
    // 216 actual pixels
    // 18 columns, 40 rows
    m_numPixels = 216;
    m_pixelBufferSize = std::max((uint) PB_MINIMUM_PIXELS, m_numPixels);
    m_pixelColors = new uint32_t[m_pixelBufferSize];

    

}