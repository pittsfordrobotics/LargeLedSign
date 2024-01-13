#include <Adafruit_NeoPixel.h>
#include <vector>
#include "Arduino.h"
#include "PixelBuffer.h"

// All pixel buffers will have at least this many pixels, even if not used.
// This is done to help the signs stay in sync.
uint minimumPixelsInBuffer = 360;

PixelBuffer::PixelBuffer(int gpioPin)
{
    m_gpioPin = gpioPin;
}

void PixelBuffer::initialize(byte signStyle)
{
    randomSeed(signStyle * 1000 + millis());
    Serial.print("Initializing pixel buffer for style ");
    Serial.println(signStyle);

    switch (signStyle)
    {
        case 1:
            initializeDigitOne();
            break;
        case 3:
            initializeDigitThree();
            break;
        case 8:
            initializeDigitEight();
            break;
        default:
            initializeTestMatrix();
    }

    clearBuffer();
    m_neoPixels->begin();
    m_neoPixels->clear();
}

void PixelBuffer::clearBuffer()
{
    for (uint i = 0; i < m_numPixels; i++)
    {
        m_pixelColors[i] = 0;
    }
}

void PixelBuffer::displayPixels()
{
    for (uint i = 0; i < m_numPixels; i++)
    {
        m_neoPixels->setPixelColor(i, m_pixelColors[i]);
    }

    unsigned long start = millis();
    m_neoPixels->show();

    // I have no idea why, but if we exit immediately and try to read BLE settings,
    // the BLE readings are sometimes corrupt.  If we wait until the "show" is done
    // and delay a tiny bit more, things are stable.
    while (!m_neoPixels->canShow())
    {
        // wait for the "show" to complete
    }
    while (millis() - start < 10)
    {
        // wait until at least 10 msec have passed since starting the "show"
    }
}

uint PixelBuffer::getColumnCount()
{
    return m_columns.size();
}

uint PixelBuffer::getRowCount()
{
    return m_rows.size();
}

uint PixelBuffer::getPixelCount()
{
    return m_numPixels;
}

void PixelBuffer::setBrightness(byte brightness)
{
    m_neoPixels->setBrightness(brightness);
}

void PixelBuffer::setPixel(unsigned int pixel, ulong color)
{
    if (pixel >= m_numPixels)
    {
        return;
    }

    m_pixelColors[pixel] = color;
}

void PixelBuffer::fill(ulong newColor)
{
    for (uint i = 0; i < m_numPixels; i++)
    {
        m_pixelColors[i] = newColor;
    }
}

void PixelBuffer::fillRandomly(ulong newColor, uint numberOfPixels)
{
    for (uint i = 0; i < numberOfPixels; i++)
    {
        int pixel = random(0, m_numPixels);
        m_pixelColors[pixel] = newColor;
    }
}

void PixelBuffer::shiftPixelsRight(ulong newColor)
{
    for (uint i = m_numPixels - 1; i >= 1; i--)
    {
        m_pixelColors[i] = m_pixelColors[i - 1];
    }

    m_pixelColors[0] = newColor;
}

void PixelBuffer::shiftPixelsLeft(ulong newColor)
{
    for (uint i = 0; i < m_numPixels - 1; i++)
    {
        m_pixelColors[i] = m_pixelColors[i + 1];
    }

    m_pixelColors[m_numPixels - 1] = newColor;
}

void PixelBuffer::shiftColumnsRight(ulong newColor)
{
    shiftPixelBlocksRight(m_columns, newColor, 0);
}

void PixelBuffer::shiftColumnsRight(ulong newColor, uint startingColumn)
{
    shiftPixelBlocksRight(m_columns, newColor, startingColumn);
}

void PixelBuffer::shiftColumnsLeft(ulong newColor)
{
    shiftPixelBlocksLeft(m_columns, newColor, m_columns.size() - 1);
}

void PixelBuffer::shiftColumnsLeft(ulong newColor, uint startingColumn)
{
    shiftPixelBlocksLeft(m_columns, newColor, startingColumn);
}

void PixelBuffer::shiftRowsUp(ulong newColor)
{
    shiftPixelBlocksLeft(m_rows, newColor, m_rows.size() - 1);
}

void PixelBuffer::shiftRowsUp(ulong newColor, uint startingRow)
{
    shiftPixelBlocksLeft(m_rows, newColor, startingRow);
}

void PixelBuffer::shiftRowsDown(ulong newColor)
{
    shiftPixelBlocksRight(m_rows, newColor, 0);
}

void PixelBuffer::shiftRowsDown(ulong newColor, uint startingRow)
{
    shiftPixelBlocksRight(m_rows, newColor, startingRow);
}

void PixelBuffer::shiftPixelBlocksRight(std::vector<std::vector<int> *> pixelBlocks, ulong newColor, uint startingBlock)
{
    for (uint i = pixelBlocks.size() - 1; i > startingBlock; i--)
    {
        std::vector<int> *source = pixelBlocks.at(i - 1);
        std::vector<int> *destination = pixelBlocks.at(i);
        // Find the color of the first pixel in the source column, and set the destination column to that color.
        uint32_t previousColor = m_pixelColors[source->at(0)];
        setColorForMappedPixels(destination, previousColor);
    }

    setColorForMappedPixels(pixelBlocks.at(startingBlock), newColor);
}

void PixelBuffer::shiftPixelBlocksLeft(std::vector<std::vector<int> *> pixelBlocks, ulong newColor, uint startingBlock)
{
    for (uint i = 0; i < startingBlock; i++)
    {
        std::vector<int> *source = pixelBlocks.at(i + 1);
        std::vector<int> *destination = pixelBlocks.at(i);
        // Find the color of the first pixel in the source column, and set the destination column to that color.
        uint32_t previousColor = m_pixelColors[source->at(0)];
        setColorForMappedPixels(destination, previousColor);
    }

    setColorForMappedPixels(pixelBlocks.at(startingBlock), newColor);
}

void PixelBuffer::setColorForMappedPixels(std::vector<int> *destination, uint32_t newColor)
{
    for (uint i = 0; i < destination->size(); i++)
    {
        int pixelIndex = destination->at(i);
        m_pixelColors[pixelIndex] = newColor;
    }
}

void PixelBuffer::initializeTestMatrix()
{
    m_numPixels = std::max(minimumPixelsInBuffer, (uint)64);
    m_pixelColors = new uint32_t[m_numPixels];
    m_neoPixels = new Adafruit_NeoPixel(m_numPixels, m_gpioPin, NEO_GRB + NEO_KHZ800);

    // Map the pixel indices to rows, columns.
    // ROW 0 is at the TOP of the display.
    // COLUMN 0 is at the LEFT of the display.
    for (int col = 7; col >= 0; col--)
    {
        std::vector<int> *rowVector = new std::vector<int>();
        for (int row = 0; row < 8; row++)
        {
            if (row % 2 == 0)
            {
                rowVector->push_back(row * 8 + col);
            }
            else
            {
                rowVector->push_back(row * 8 + (7 - col));
            }
        }
        m_rows.push_back(rowVector);
    }

    for (int col = 7; col >= 0; col--)
    {
        std::vector<int> *colVector = new std::vector<int>();
        for (int row = 0; row < 8; row++)
        {
            colVector->push_back(col * 8 + row);
        }
        m_columns.push_back(colVector);
    }
}

void PixelBuffer::initializeDigitOne()
{
    // 216 actual pixels (0-215)
    // 18 columns, 40 rows
    m_numPixels = std::max(minimumPixelsInBuffer, (uint)216);
    m_pixelColors = new uint32_t[m_numPixels];
    m_neoPixels = new Adafruit_NeoPixel(m_numPixels, m_gpioPin, NEO_GRB + NEO_KHZ800);

    m_columns.push_back(new std::vector<int>{ 180, 189, 198 });
    m_columns.push_back(new std::vector<int>{ 185, 194, 203 });
    m_columns.push_back(new std::vector<int>{ 181, 190, 199 });
    m_columns.push_back(new std::vector<int>{ 186, 195, 204 });
    m_columns.push_back(new std::vector<int>{ 182, 191, 200, 207 });
    m_columns.push_back(new std::vector<int>{ 187, 196, 205, 210 });
    m_columns.push_back(new std::vector<int>{ 183, 192, 201, 208, 212 });
    m_columns.push_back(new std::vector<int>{ 188, 197, 206, 211, 214 });
    m_columns.push_back(new std::vector<int>{ 184, 193, 202, 209, 213, 215 });
    m_columns.push_back(new std::vector<int>{ 0, 9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 108, 117, 126, 135, 144, 153, 162, 171 });
    m_columns.push_back(new std::vector<int>{ 5, 14, 23, 32, 41, 50, 59, 68, 77, 86, 95, 104, 113, 122, 131, 140, 149, 158, 167, 176 });
    m_columns.push_back(new std::vector<int>{ 1, 10, 19, 28, 37, 46, 55, 64, 73, 82, 91, 100, 109, 118, 127, 136, 145, 154, 163, 172 });
    m_columns.push_back(new std::vector<int>{ 6, 15, 24, 33, 42, 51, 60, 69, 78, 87, 96, 105, 114, 123, 132, 141, 150, 159, 168, 177 });
    m_columns.push_back(new std::vector<int>{ 2, 11, 20, 29, 38, 47, 56, 65, 74, 83, 92, 101, 110, 119, 128, 137, 146, 155, 164, 173 });
    m_columns.push_back(new std::vector<int>{ 7, 16, 25, 34, 43, 52, 61, 70, 79, 88, 97, 106, 115, 124, 133, 142, 151, 160, 169, 178 });
    m_columns.push_back(new std::vector<int>{ 3, 12, 21, 30, 39, 48, 57, 66, 75, 84, 93, 102, 111, 120, 129, 138, 147, 156, 165, 174 });
    m_columns.push_back(new std::vector<int>{ 8, 17, 26, 35, 44, 53, 62, 71, 80, 89, 98, 107, 116, 125, 134, 143, 152, 161, 170, 179 });
    m_columns.push_back(new std::vector<int>{ 4, 13, 22, 31, 40, 49, 58, 67, 76, 85, 94, 103, 112, 121, 130, 139, 148, 157, 166, 175 });

    m_rows.push_back(new std::vector<int>{ 176, 177, 178, 179 });
    m_rows.push_back(new std::vector<int>{ 171, 172, 173, 174, 175 });
    m_rows.push_back(new std::vector<int>{ 167, 168, 169, 170, 215 });
    m_rows.push_back(new std::vector<int>{ 162, 163, 164, 165, 166, 214 });
    m_rows.push_back(new std::vector<int>{ 158, 159, 160, 161, 212, 213 });
    m_rows.push_back(new std::vector<int>{ 153, 154, 155, 156, 157, 210, 211 });
    m_rows.push_back(new std::vector<int>{ 149, 150, 151, 152, 207, 208, 209 });
    m_rows.push_back(new std::vector<int>{ 144, 145, 146, 147, 148, 203, 204, 205, 206 });
    m_rows.push_back(new std::vector<int>{ 140, 141, 142, 143, 198, 199, 200, 201, 202 });
    m_rows.push_back(new std::vector<int>{ 135, 136, 137, 138, 139, 194, 195, 196, 197 });
    m_rows.push_back(new std::vector<int>{ 131, 132, 133, 134, 189, 190, 191, 192, 193 });
    m_rows.push_back(new std::vector<int>{ 126, 127, 128, 129, 130, 185, 186, 187, 188 });
    m_rows.push_back(new std::vector<int>{ 122, 123, 124, 125, 180, 181, 182, 183, 184 });
    m_rows.push_back(new std::vector<int>{ 117, 118, 119, 120, 121 });
    m_rows.push_back(new std::vector<int>{ 113, 114, 115, 116 });
    m_rows.push_back(new std::vector<int>{ 108, 109, 110, 111, 112 });
    m_rows.push_back(new std::vector<int>{ 104, 105, 106, 107 });
    m_rows.push_back(new std::vector<int>{ 99, 100, 101, 102, 103 });
    m_rows.push_back(new std::vector<int>{ 95, 96, 97, 98 });
    m_rows.push_back(new std::vector<int>{ 90, 91, 92, 93, 94 });
    m_rows.push_back(new std::vector<int>{ 86, 87, 88, 89 });
    m_rows.push_back(new std::vector<int>{ 81, 82, 83, 84, 85 });
    m_rows.push_back(new std::vector<int>{ 77, 78, 79, 80 });
    m_rows.push_back(new std::vector<int>{ 72, 73, 74, 75, 76 });
    m_rows.push_back(new std::vector<int>{ 68, 69, 70, 71 });
    m_rows.push_back(new std::vector<int>{ 63, 64, 65, 66, 67 });
    m_rows.push_back(new std::vector<int>{ 59, 60, 61, 62 });
    m_rows.push_back(new std::vector<int>{ 54, 55, 56, 57, 58 });
    m_rows.push_back(new std::vector<int>{ 50, 51, 52, 53 });
    m_rows.push_back(new std::vector<int>{ 45, 46, 47, 48, 49 });
    m_rows.push_back(new std::vector<int>{ 41, 42, 43, 44 });
    m_rows.push_back(new std::vector<int>{ 36, 37, 38, 39, 40 });
    m_rows.push_back(new std::vector<int>{ 32, 33, 34, 35 });
    m_rows.push_back(new std::vector<int>{ 27, 28, 29, 30, 31 });
    m_rows.push_back(new std::vector<int>{ 23, 24, 25, 26 });
    m_rows.push_back(new std::vector<int>{ 18, 19, 20, 21, 22 });
    m_rows.push_back(new std::vector<int>{ 14, 15, 16, 17 });
    m_rows.push_back(new std::vector<int>{ 9, 10, 11, 12, 13 });
    m_rows.push_back(new std::vector<int>{ 5, 6, 7, 8 });
    m_rows.push_back(new std::vector<int>{ 0, 1, 2, 3, 4 });
}

void PixelBuffer::initializeDigitThree()
{
    initializeTestMatrix();
}

void PixelBuffer::initializeDigitEight()
{
    initializeTestMatrix();
}

void PixelBuffer::initializeLogo()
{
    initializeTestMatrix();
}
