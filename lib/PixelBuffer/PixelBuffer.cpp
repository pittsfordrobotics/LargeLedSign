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
    // 216 actual pixels
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
    // 313 actual pixels
    // 23 columns, 39 rows
    m_numPixels = std::max(minimumPixelsInBuffer, (uint)216);
    m_pixelColors = new uint32_t[m_numPixels];
    m_neoPixels = new Adafruit_NeoPixel(m_numPixels, m_gpioPin, NEO_GRB + NEO_KHZ800);

    m_columns.push_back(new std::vector<int>{ 0, 9, 18, 27, 36, 253, 262, 271, 280, 289});
    m_columns.push_back(new std::vector<int>{ 5, 14, 23, 32, 42, 258, 267, 276, 285, 295});
    m_columns.push_back(new std::vector<int>{ 1, 10, 19, 28, 37, 47, 254, 263, 272, 281, 290, 300});
    m_columns.push_back(new std::vector<int>{ 6, 15, 24, 33, 43, 52, 259, 268, 277, 286, 296, 305});
    m_columns.push_back(new std::vector<int>{ 2, 11, 20, 29, 38, 48, 56, 255, 264, 273, 282, 291, 301, 309});
    m_columns.push_back(new std::vector<int>{ 7, 16, 25, 34, 44, 53, 260, 269, 278, 287, 297, 306});
    m_columns.push_back(new std::vector<int>{ 3, 12, 21, 30, 39, 49, 57, 256, 265, 274, 283, 292, 302, 310});
    m_columns.push_back(new std::vector<int>{ 8, 17, 26, 35, 45, 54, 141, 154, 165, 261, 270, 279, 288, 298, 307});
    m_columns.push_back(new std::vector<int>{ 4, 13, 22, 31, 40, 50, 58, 148, 159, 257, 266, 275, 284, 293, 303, 311});
    m_columns.push_back(new std::vector<int>{ 46, 55, 142, 155, 166, 299, 308});
    m_columns.push_back(new std::vector<int>{ 41, 51, 59, 149, 160, 294, 304, 312});
    m_columns.push_back(new std::vector<int>{ 60, 61, 143, 156, 167, 251, 252});
    m_columns.push_back(new std::vector<int>{ 103, 113, 121, 136, 150, 161, 172, 232, 242, 250});
    m_columns.push_back(new std::vector<int>{ 108, 117, 131, 144, 157, 168, 177, 237, 246});
    m_columns.push_back(new std::vector<int>{ 66, 75, 84, 93, 102, 112, 120, 126, 137, 151, 162, 173, 182, 195, 204, 213, 222, 231, 241, 249});
    m_columns.push_back(new std::vector<int>{ 70, 79, 88, 97, 107, 116, 122, 132, 145, 158, 169, 178, 187, 199, 208, 217, 226, 236, 245});
    m_columns.push_back(new std::vector<int>{ 65, 74, 83, 92, 101, 111, 119, 127, 138, 152, 163, 174, 183, 194, 203, 212, 221, 230, 240, 248});
    m_columns.push_back(new std::vector<int>{ 69, 78, 87, 96, 106, 115, 123, 133, 146, 170, 179, 188, 198, 207, 216, 225, 235, 244});
    m_columns.push_back(new std::vector<int>{ 64, 73, 82, 91, 100, 110, 118, 128, 139, 153, 164, 175, 184, 193, 202, 211, 220, 229, 239, 247});
    m_columns.push_back(new std::vector<int>{ 68, 77, 86, 95, 105, 114, 124, 134, 147, 171, 180, 189, 197, 206, 215, 224, 234, 243});
    m_columns.push_back(new std::vector<int>{ 63, 72, 81, 90, 99, 109, 129, 140, 176, 185, 192, 201, 210, 219, 228, 238});
    m_columns.push_back(new std::vector<int>{ 67, 76, 85, 94, 104, 125, 135, 181, 190, 196, 205, 214, 223, 233});
    m_columns.push_back(new std::vector<int>{ 62, 71, 80, 89, 98, 130, 186, 191, 200, 209, 218, 227});

    m_rows.push_back(new std::vector<int>{ 247, 248, 249, 250, 309, 310, 311, 312});
    m_rows.push_back(new std::vector<int>{ 243, 244, 245, 246, 251, 305, 306, 307, 308});
    m_rows.push_back(new std::vector<int>{ 238, 239, 240, 241, 242, 300, 301, 302, 303, 304});
    m_rows.push_back(new std::vector<int>{ 233, 234, 235, 236, 237, 252, 295, 296, 297, 298, 299});
    m_rows.push_back(new std::vector<int>{ 227, 228, 229, 230, 231, 232, 289, 290, 291, 292, 293, 294});
    m_rows.push_back(new std::vector<int>{ 223, 224, 225, 226, 285, 286, 287, 288});
    m_rows.push_back(new std::vector<int>{ 218, 219, 220, 221, 222, 280, 281, 282, 283, 284});
    m_rows.push_back(new std::vector<int>{ 214, 215, 216, 217, 276, 277, 278, 279});
    m_rows.push_back(new std::vector<int>{ 209, 210, 211, 212, 213, 271, 272, 273, 274, 275});
    m_rows.push_back(new std::vector<int>{ 205, 206, 207, 208, 267, 268, 269, 270});
    m_rows.push_back(new std::vector<int>{ 200, 201, 202, 203, 204, 262, 263, 264, 265, 266});
    m_rows.push_back(new std::vector<int>{ 196, 197, 198, 199, 258, 259, 260, 261});
    m_rows.push_back(new std::vector<int>{ 191, 192, 193, 194, 195, 253, 254, 255, 256, 257});
    m_rows.push_back(new std::vector<int>{ 122, 123, 124, 125});
    m_rows.push_back(new std::vector<int>{ 126, 127, 128, 129, 130});
    m_rows.push_back(new std::vector<int>{ 131, 132, 133, 134, 135});
    m_rows.push_back(new std::vector<int>{ 136, 137, 138, 139, 140});
    m_rows.push_back(new std::vector<int>{ 141, 142, 143, 144, 145, 146, 147});
    m_rows.push_back(new std::vector<int>{ 148, 149, 150, 151, 152, 153});
    m_rows.push_back(new std::vector<int>{ 154, 155, 156, 157, 158});
    m_rows.push_back(new std::vector<int>{ 159, 160, 161, 162, 163, 164});
    m_rows.push_back(new std::vector<int>{ 165, 166, 167, 168, 169, 170, 171});
    m_rows.push_back(new std::vector<int>{ 172, 173, 174, 175, 176});
    m_rows.push_back(new std::vector<int>{ 177, 178, 179, 180, 181});
    m_rows.push_back(new std::vector<int>{ 182, 183, 184, 185, 186});
    m_rows.push_back(new std::vector<int>{ 187, 188, 189, 190});
    m_rows.push_back(new std::vector<int>{ 0, 1, 2, 3, 4, 62, 63, 64, 65, 66});
    m_rows.push_back(new std::vector<int>{ 5, 6, 7, 8, 67, 68, 69, 70});
    m_rows.push_back(new std::vector<int>{ 9, 10, 11, 12, 13, 71, 72, 73, 74, 75});
    m_rows.push_back(new std::vector<int>{ 14, 15, 16, 17, 76, 77, 78, 79});
    m_rows.push_back(new std::vector<int>{ 18, 19, 20, 21, 22, 80, 81, 82, 83, 84});
    m_rows.push_back(new std::vector<int>{ 23, 24, 25, 26, 85, 86, 87, 88});
    m_rows.push_back(new std::vector<int>{ 27, 28, 29, 30, 31, 89, 90, 91, 92, 93});
    m_rows.push_back(new std::vector<int>{ 32, 33, 34, 35, 94, 95, 96, 97});
    m_rows.push_back(new std::vector<int>{ 36, 37, 38, 39, 40, 41, 98, 99, 100, 101, 102, 103});
    m_rows.push_back(new std::vector<int>{ 42, 43, 44, 45, 46, 61, 104, 105, 106, 107, 108});
    m_rows.push_back(new std::vector<int>{ 47, 48, 49, 50, 51, 109, 110, 111, 112, 113});
    m_rows.push_back(new std::vector<int>{ 52, 53, 54, 55, 60, 114, 115, 116, 117});
    m_rows.push_back(new std::vector<int>{ 56, 57, 58, 59, 118, 119, 120, 121});
}

void PixelBuffer::initializeDigitEight()
{
    // 356 actual pixels
    // 23 columns, 39 rows
    m_numPixels = std::max(minimumPixelsInBuffer, (uint)216);
    m_pixelColors = new uint32_t[m_numPixels];
    m_neoPixels = new Adafruit_NeoPixel(m_numPixels, m_gpioPin, NEO_GRB + NEO_KHZ800);

    m_columns.push_back(new std::vector<int>{ 0, 9, 18, 27, 36, 240, 249, 258, 267, 276, 308, 351});
    m_columns.push_back(new std::vector<int>{ 5, 14, 23, 32, 42, 245, 254, 263, 272, 282, 303, 313, 346, 355});
    m_columns.push_back(new std::vector<int>{ 1, 10, 19, 28, 37, 47, 241, 250, 259, 268, 277, 287, 307, 318, 341, 350});
    m_columns.push_back(new std::vector<int>{ 6, 15, 24, 33, 43, 52, 246, 255, 264, 273, 283, 292, 302, 312, 322, 336, 345, 354});
    m_columns.push_back(new std::vector<int>{ 2, 11, 20, 29, 38, 48, 56, 242, 251, 260, 269, 278, 288, 296, 306, 317, 326, 332, 340, 349});
    m_columns.push_back(new std::vector<int>{ 7, 16, 25, 34, 44, 53, 247, 256, 265, 274, 284, 293, 301, 311, 321, 335, 344, 353});
    m_columns.push_back(new std::vector<int>{ 3, 12, 21, 30, 39, 49, 57, 243, 252, 261, 270, 279, 289, 297, 305, 316, 325, 331, 339, 348});
    m_columns.push_back(new std::vector<int>{ 8, 17, 26, 35, 45, 54, 248, 257, 266, 275, 285, 294, 300, 310, 320, 328, 334, 343, 352});
    m_columns.push_back(new std::vector<int>{ 4, 13, 22, 31, 40, 50, 58, 244, 253, 262, 271, 280, 290, 298, 304, 315, 324, 330, 338, 347});
    m_columns.push_back(new std::vector<int>{ 46, 55, 286, 295, 309, 319, 327, 333, 342});
    m_columns.push_back(new std::vector<int>{ 41, 51, 59, 281, 291, 299, 314, 323, 329, 337});
    m_columns.push_back(new std::vector<int>{ 60, 61, 238, 239});
    m_columns.push_back(new std::vector<int>{ 103, 113, 121, 136, 145, 151, 159, 219, 229, 237});
    m_columns.push_back(new std::vector<int>{ 108, 117, 131, 141, 149, 155, 164, 224, 233});
    m_columns.push_back(new std::vector<int>{ 66, 75, 84, 93, 102, 112, 120, 126, 137, 146, 152, 160, 169, 182, 191, 200, 209, 218, 228, 236});
    m_columns.push_back(new std::vector<int>{ 70, 79, 88, 97, 107, 116, 122, 132, 142, 150, 156, 165, 174, 186, 195, 204, 213, 223, 232});
    m_columns.push_back(new std::vector<int>{ 65, 74, 83, 92, 101, 111, 119, 127, 138, 147, 153, 161, 170, 181, 190, 199, 208, 217, 227, 235});
    m_columns.push_back(new std::vector<int>{ 69, 78, 87, 96, 106, 115, 123, 133, 143, 157, 166, 175, 185, 194, 203, 212, 222, 231});
    m_columns.push_back(new std::vector<int>{ 64, 73, 82, 91, 100, 110, 118, 128, 139, 148, 154, 162, 171, 180, 189, 198, 207, 216, 226, 234});
    m_columns.push_back(new std::vector<int>{ 68, 77, 86, 95, 105, 114, 124, 134, 144, 158, 167, 176, 184, 193, 202, 211, 221, 230});
    m_columns.push_back(new std::vector<int>{ 63, 72, 81, 90, 99, 109, 129, 140, 163, 172, 179, 188, 197, 206, 215, 225});
    m_columns.push_back(new std::vector<int>{ 67, 76, 85, 94, 104, 125, 135, 168, 177, 183, 192, 201, 210, 220});
    m_columns.push_back(new std::vector<int>{ 62, 71, 80, 89, 98, 130, 173, 178, 187, 196, 205, 214});

    m_rows.push_back(new std::vector<int>{ 234, 235, 236, 237, 296, 297, 298, 299});
    m_rows.push_back(new std::vector<int>{ 230, 231, 232, 233, 238, 292, 293, 294, 295});
    m_rows.push_back(new std::vector<int>{ 225, 226, 227, 228, 229, 287, 288, 289, 290, 291});
    m_rows.push_back(new std::vector<int>{ 220, 221, 222, 223, 224, 239, 282, 283, 284, 285, 286});
    m_rows.push_back(new std::vector<int>{ 214, 215, 216, 217, 218, 219, 276, 277, 278, 279, 280, 281});
    m_rows.push_back(new std::vector<int>{ 210, 211, 212, 213, 272, 273, 274, 275});
    m_rows.push_back(new std::vector<int>{ 205, 206, 207, 208, 209, 267, 268, 269, 270, 271});
    m_rows.push_back(new std::vector<int>{ 201, 202, 203, 204, 263, 264, 265, 266});
    m_rows.push_back(new std::vector<int>{ 196, 197, 198, 199, 200, 258, 259, 260, 261, 262});
    m_rows.push_back(new std::vector<int>{ 192, 193, 194, 195, 254, 255, 256, 257});
    m_rows.push_back(new std::vector<int>{ 187, 188, 189, 190, 191, 249, 250, 251, 252, 253});
    m_rows.push_back(new std::vector<int>{ 183, 184, 185, 186, 245, 246, 247, 248});
    m_rows.push_back(new std::vector<int>{ 178, 179, 180, 181, 182, 240, 241, 242, 243, 244});
    m_rows.push_back(new std::vector<int>{ 122, 123, 124, 125, 300, 301, 302, 303});
    m_rows.push_back(new std::vector<int>{ 126, 127, 128, 129, 130, 304, 305, 306, 307, 308});
    m_rows.push_back(new std::vector<int>{ 131, 132, 133, 134, 135, 309, 310, 311, 312, 313});
    m_rows.push_back(new std::vector<int>{ 136, 137, 138, 139, 140, 314, 315, 316, 317, 318});
    m_rows.push_back(new std::vector<int>{ 141, 142, 143, 144, 319, 320, 321, 322});
    m_rows.push_back(new std::vector<int>{ 145, 146, 147, 148, 323, 324, 325, 326});
    m_rows.push_back(new std::vector<int>{ 149, 150, 327, 328});
    m_rows.push_back(new std::vector<int>{ 151, 152, 153, 154, 329, 330, 331, 332});
    m_rows.push_back(new std::vector<int>{ 155, 156, 157, 158, 333, 334, 335, 336});
    m_rows.push_back(new std::vector<int>{ 159, 160, 161, 162, 163, 337, 338, 339, 340, 341});
    m_rows.push_back(new std::vector<int>{ 164, 165, 166, 167, 168, 342, 343, 344, 345, 346});
    m_rows.push_back(new std::vector<int>{ 169, 170, 171, 172, 173, 347, 348, 349, 350, 351});
    m_rows.push_back(new std::vector<int>{ 174, 175, 176, 177, 352, 353, 354, 355});
    m_rows.push_back(new std::vector<int>{ 0, 1, 2, 3, 4, 62, 63, 64, 65, 66});
    m_rows.push_back(new std::vector<int>{ 5, 6, 7, 8, 67, 68, 69, 70});
    m_rows.push_back(new std::vector<int>{ 9, 10, 11, 12, 13, 71, 72, 73, 74, 75});
    m_rows.push_back(new std::vector<int>{ 14, 15, 16, 17, 76, 77, 78, 79});
    m_rows.push_back(new std::vector<int>{ 18, 19, 20, 21, 22, 80, 81, 82, 83, 84});
    m_rows.push_back(new std::vector<int>{ 23, 24, 25, 26, 85, 86, 87, 88});
    m_rows.push_back(new std::vector<int>{ 27, 28, 29, 30, 31, 89, 90, 91, 92, 93});
    m_rows.push_back(new std::vector<int>{ 32, 33, 34, 35, 94, 95, 96, 97});
    m_rows.push_back(new std::vector<int>{ 36, 37, 38, 39, 40, 41, 98, 99, 100, 101, 102, 103});
    m_rows.push_back(new std::vector<int>{ 42, 43, 44, 45, 46, 61, 104, 105, 106, 107, 108});
    m_rows.push_back(new std::vector<int>{ 47, 48, 49, 50, 51, 109, 110, 111, 112, 113});
    m_rows.push_back(new std::vector<int>{ 52, 53, 54, 55, 60, 114, 115, 116, 117});
    m_rows.push_back(new std::vector<int>{ 56, 57, 58, 59, 118, 119, 120, 121});
}

void PixelBuffer::initializeLogo()
{
    // 140 actual pixels
    // 23 columns, 24 rows

    m_numPixels = std::max(minimumPixelsInBuffer, (uint)140);
    m_pixelColors = new uint32_t[m_numPixels];
    m_neoPixels = new Adafruit_NeoPixel(m_numPixels, m_gpioPin, NEO_GRB + NEO_KHZ800);

    m_columns.push_back(new std::vector<int>{ 99, 100});
    m_columns.push_back(new std::vector<int>{ 98, 102});
    m_columns.push_back(new std::vector<int>{ 95, 96, 97, 101, 103, 104});
    m_columns.push_back(new std::vector<int>{ 91, 93, 94, 105, 106, 107, 109});
    m_columns.push_back(new std::vector<int>{ 89, 90, 92, 108, 110, 111});
    m_columns.push_back(new std::vector<int>{ 7, 88, 112});
    m_columns.push_back(new std::vector<int>{ 13, 25, 87, 113});
    m_columns.push_back(new std::vector<int>{ 8, 19, 26, 86, 114});
    m_columns.push_back(new std::vector<int>{ 2, 9, 14, 27, 31, 33, 85, 115});
    m_columns.push_back(new std::vector<int>{ 0, 20, 29, 34, 35, 84, 116});
    m_columns.push_back(new std::vector<int>{ 3, 15, 28, 32, 82, 83, 117, 118, 119});
    m_columns.push_back(new std::vector<int>{ 1, 10, 21, 30, 52, 78, 80, 81, 120, 121});
    m_columns.push_back(new std::vector<int>{ 4, 16, 36, 47, 57, 77, 79, 122, 123});
    m_columns.push_back(new std::vector<int>{ 11, 22, 42, 53, 61, 68, 124});
    m_columns.push_back(new std::vector<int>{ 5, 17, 37, 48, 58, 65, 76, 125});
    m_columns.push_back(new std::vector<int>{ 12, 23, 43, 54, 62, 69, 73, 126});
    m_columns.push_back(new std::vector<int>{ 6, 18, 38, 49, 59, 66, 74, 127});
    m_columns.push_back(new std::vector<int>{ 24, 44, 55, 63, 70, 128});
    m_columns.push_back(new std::vector<int>{ 39, 50, 60, 71, 75, 129, 130, 132});
    m_columns.push_back(new std::vector<int>{ 45, 56, 64, 67, 131, 133, 134, 135});
    m_columns.push_back(new std::vector<int>{ 40, 51, 72, 136, 137});
    m_columns.push_back(new std::vector<int>{ 46, 138});
    m_columns.push_back(new std::vector<int>{ 41, 139});

    m_rows.push_back(new std::vector<int>{ 119, 120});
    m_rows.push_back(new std::vector<int>{ 118, 122});
    m_rows.push_back(new std::vector<int>{ 117, 121, 124});
    m_rows.push_back(new std::vector<int>{ 111, 115, 116, 123, 125, 129});
    m_rows.push_back(new std::vector<int>{ 113, 114, 126, 127});
    m_rows.push_back(new std::vector<int>{ 109, 110, 112, 128, 130, 131});
    m_rows.push_back(new std::vector<int>{ 108, 132});
    m_rows.push_back(new std::vector<int>{ 0, 1, 107, 133});
    m_rows.push_back(new std::vector<int>{ 2, 3, 4, 5, 6, 106, 134});
    m_rows.push_back(new std::vector<int>{ 7, 8, 9, 10, 11, 12, 105, 135});
    m_rows.push_back(new std::vector<int>{ 13, 14, 15, 16, 17, 18, 104, 136});
    m_rows.push_back(new std::vector<int>{ 19, 20, 21, 22, 23, 24, 25, 27, 102, 103, 137, 139});
    m_rows.push_back(new std::vector<int>{ 26, 28, 36, 37, 38, 39, 40, 41, 98, 100, 101, 138});
    m_rows.push_back(new std::vector<int>{ 29, 30, 42, 43, 44, 45, 46, 97, 99});
    m_rows.push_back(new std::vector<int>{ 31, 32, 47, 48, 49, 50, 51, 96});
    m_rows.push_back(new std::vector<int>{ 33, 34, 52, 53, 54, 55, 56, 95});
    m_rows.push_back(new std::vector<int>{ 35, 57, 58, 59, 60, 94});
    m_rows.push_back(new std::vector<int>{ 61, 62, 63, 64, 93});
    m_rows.push_back(new std::vector<int>{ 65, 66, 67, 92});
    m_rows.push_back(new std::vector<int>{ 68, 69, 70, 71, 72, 88, 90, 91});
    m_rows.push_back(new std::vector<int>{ 73, 74, 75, 86, 87});
    m_rows.push_back(new std::vector<int>{ 76, 77, 81, 83, 84, 85, 89});
    m_rows.push_back(new std::vector<int>{ 78, 82});
    m_rows.push_back(new std::vector<int>{ 79, 80});
}
