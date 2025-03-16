#include "FireDisplayPattern.h"

FireDisplayPattern::FireDisplayPattern(PixelBuffer *pixelBuffer) : DisplayPattern(pixelBuffer)
{
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
    // Reference: https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/#LEDStripEffectFire
    //
    // From the site:
    // Cooling indicates how fast a flame cools down. More cooling means shorter flames, and the recommended 
    // values are between 20 and 100. 50 seems the nicest.
    //
    // Sparking indicates the chance (out of 255) that a spark will ignite. A higher value makes the fire 
    // more active. Suggested values lay between 50 and 200, with my personal preference being 120.
    //

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
        // heat[y] = random(160,255);
    }

    // Step 4.  Convert heat to LED colors
    for (int j = 0; j < numRows; j++)
    {
        setRowHeatColor(j, m_rowHeats[j]);
    }
}

void FireDisplayPattern::setRowHeatColor(int row, byte temperature)
{
    // Scale 'heat' down from 0-255 to 0-191
    byte t192 = round((temperature / 255.0) * 191);

    // calculate ramp up from
    byte heatramp = t192 & 0x3F; // 0..63
    heatramp <<= 2;              // scale up to 0..252

    byte red;
    byte green;
    byte blue;

    // figure out which third of the spectrum we're in:
    if (t192 > 0x80)
    { // hottest
        red = 255;
        green = 255;
        blue = heatramp;
    }
    else if (t192 > 0x40)
    { // middle
        red = 255;
        green = heatramp;
        blue = 0;
    }
    else
    { // coolest
        red = heatramp;
        green = 0;
        blue = 0;
    }

    ulong color = Adafruit_NeoPixel::Color(red, green, blue);
    m_pixelBuffer->setRowColor(row, color);
}

std::vector<String> FireDisplayPattern::getParameterNames()
{
    std::vector<String> parameterNames;
    parameterNames.push_back("Sparking");
    parameterNames.push_back("Cooling");

    return parameterNames;
}