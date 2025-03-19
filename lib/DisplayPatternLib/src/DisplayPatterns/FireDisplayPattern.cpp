#include "FireDisplayPattern.h"

FireDisplayPattern::FireDisplayPattern(PixelBuffer *pixelBuffer, bool useIndividualDigits) : DisplayPattern(pixelBuffer)
{
    m_useIndividualDigits = useIndividualDigits;
    GeneratePallet();
    // Ensure we call reset at least once to set up the row heats list.
    m_rowsForDigits.clear();
    
    for (uint digitNumber = 0; digitNumber < pixelBuffer->getDigitCount(); digitNumber++)
    {
        m_rowsForDigits.push_back(pixelBuffer->getRowsForDigit(digitNumber));
    }

    resetInternal();
}

void FireDisplayPattern::setSparkingAmount(byte sparkingAmount)
{
    m_sparking = sparkingAmount;
}

void FireDisplayPattern::setCoolingAmount(byte coolingAmount)
{
    m_cooling = coolingAmount;
}

void FireDisplayPattern::resetInternal()
{
    m_pixelBuffer->fill(0);

    // Initialize row heats to 0
    m_rowHeats.clear();
    for (uint i = 0; i < m_pixelBuffer->getRowCount(); i++)
    {
        m_rowHeats.push_back(0);
    }

    // Initialize digit-based row heats
    m_rowHeatsPerDigit.clear();
    for (std::vector<std::vector<int>*> rowsInDigit : m_rowsForDigits)
    {
        std::vector<int> rowHeatsForDigit;
        for (uint i = 0; i < rowsInDigit.size(); i++)
        {
            rowHeatsForDigit.push_back(0);
        }

        m_rowHeatsPerDigit.push_back(rowHeatsForDigit);
    }
}

void FireDisplayPattern::updateInternal()
{
    if (m_useIndividualDigits)
    {
        for (int digit = 0; digit < m_rowHeatsPerDigit.size(); digit++)
        {
            updateRowHeats(m_rowHeatsPerDigit[digit]);
            int numRows = m_rowHeatsPerDigit[digit].size();
            for (int row = 0; row < numRows; row++)
            {
                byte temperature = m_rowHeatsPerDigit[digit][row];
                ulong color = m_heatColors[temperature];
                for (int pixel : *(m_rowsForDigits[digit][numRows - row - 1]))
                {
                    m_pixelBuffer->setPixel(pixel, color);
                }
            }
        }
    }
    else
    {
        updateRowHeats(m_rowHeats);
        int numRows = m_rowHeats.size();

        for (int j = 0; j < numRows; j++)
        {
            // Invert the rows -- the top row is 0.
            setRowHeatColor(numRows - j - 1, m_rowHeats[j]);
        }
    }
}

void FireDisplayPattern::updateRowHeats(std::vector<int> &rowHeats)
{
    // Originally based on https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/#LEDStripEffectFire
    // Colors for temperatures changed.
    //
    // From the site:
    // Cooling indicates how fast a flame cools down. More cooling means shorter flames, and the recommended 
    // values are between 20 and 100. 50 seems the nicest.
    //
    // Sparking indicates the chance (out of 255) that a spark will ignite. A higher value makes the fire 
    // more active. Suggested values lay between 50 and 200, with my personal preference being 120.

    int cooldown;
    int numRows = rowHeats.size();

    // Step 1.  Cool down every cell a little
    for (int i = 0; i < numRows; i++)
    {
        cooldown = random(0, ((m_cooling * 10) / numRows) + 2);

        if (cooldown > rowHeats[i])
        {
            rowHeats[i] = 0;
        }
        else
        {
            rowHeats[i] = rowHeats[i] - cooldown;
        }
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k = numRows - 1; k >= 2; k--)
    {
        rowHeats[k] = (rowHeats[k - 1] + rowHeats[k - 2] + rowHeats[k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' near the bottom
    if (random(255) < m_sparking)
    {
        int y = random(7);
        rowHeats[y] = rowHeats[y] + random(160, 255);
    }    
}

void FireDisplayPattern::setRowHeatColor(int row, byte temperature)
{
    ulong color = m_heatColors[temperature];
    m_pixelBuffer->setRowColor(row, color);
}

std::vector<String> FireDisplayPattern::getParameterNames()
{
    std::vector<String> parameterNames;
    parameterNames.push_back("Sparking");
    parameterNames.push_back("Cooling");

    return parameterNames;
}

void FireDisplayPattern::GeneratePallet()
{
    // generate the temperature-to-color pallet.
    for(int temperature = 0; temperature < 256; temperature++)
    {
        uint16_t hue = 0;
        byte saturation = 255;
        byte value = 255;
    
        if (temperature < 128)
        {
            // bottom half of temperature range:
            // hue = 0 (red), value scaled from 0 to 255
            // This should give black for 0, up to full red at 127.
            value = temperature * 2;    
        }
        else
        {
            // top half of temperature range:
            // hue from 0 to 8200 (yellowish), value at 255
            // This should give red at 128, up to yellow-ish at 255.
            hue = (temperature - 128) / 255.0 * 8200;
        }
    
        m_heatColors[temperature] = Adafruit_NeoPixel::ColorHSV(hue, saturation, value);
    }
}