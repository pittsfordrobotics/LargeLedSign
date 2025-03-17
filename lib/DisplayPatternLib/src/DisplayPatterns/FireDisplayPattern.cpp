#include "FireDisplayPattern.h"

FireDisplayPattern::FireDisplayPattern(PixelBuffer *pixelBuffer) : DisplayPattern(pixelBuffer)
{
    GeneratePallet();
    // Ensure we call reset at least once to set up the row heats list.
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
}

void FireDisplayPattern::updateInternal()
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
    int numRows = m_rowHeats.size();

    // Step 1.  Cool down every cell a little
    for (int i = 0; i < numRows; i++)
    {
        cooldown = random(0, ((m_cooling * 10) / numRows) + 2);

        if (cooldown > m_rowHeats[i])
        {
            m_rowHeats[i] = 0;
        }
        else
        {
            m_rowHeats[i] = m_rowHeats[i] - cooldown;
        }
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k = numRows - 1; k >= 2; k--)
    {
        m_rowHeats[k] = (m_rowHeats[k - 1] + m_rowHeats[k - 2] + m_rowHeats[k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' near the bottom
    if (random(255) < m_sparking)
    {
        int y = random(7);
        m_rowHeats[y] = m_rowHeats[y] + random(160, 255);
        // heat[y] = random(160,255);  // commented out in the original code sample
    }

    // Step 4.  Convert heat to LED colors
    for (int j = 0; j < numRows; j++)
    {
        // Invert the rows -- the top row is 0.
        setRowHeatColor(numRows - j - 1, m_rowHeats[j]);
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