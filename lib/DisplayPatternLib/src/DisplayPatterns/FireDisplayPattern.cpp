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
    // Reference: https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/#LEDStripEffectFire
    //
    // From the site:
    // Cooling indicates how fast a flame cools down. More cooling means shorter flames, and the recommended 
    // values are between 20 and 100. 50 seems the nicest.
    //
    // Sparking indicates the chance (out of 255) that a spark will ignite. A higher value makes the fire 
    // more active. Suggested values lay between 50 and 200, with my personal preference being 120.
    //
    // from lucia: cooling = 100, sparking = 100, speed = 200

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
        setRowHeatColor2(numRows - j - 1, m_rowHeats[j]);
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

void FireDisplayPattern::setRowHeatColor2(int row, byte temperature)
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

// https://github.com/SheepyChris/quickcg-sdl2/blob/master/quickcg.cpp
ulong FireDisplayPattern::HSLtoRGB(const byte hue, const byte saturation, const byte lightness)
{
    double r, g, b, h, s, l; //this function works with floats between 0 and 1
    double temp1, temp2, tempr, tempg, tempb;
    h = hue / 255.0;
    s = saturation / 255.0;
    l = lightness / 255.0;

    //If saturation is 0, the color is a shade of grey
    if (s == 0) r = g = b = l;
    //If saturation > 0, more complex calculations are needed
    else
    {
        //set the temporary values
        if (l < 0.5) temp2 = l * (1 + s);
        else temp2 = (l + s) - (l * s);
        temp1 = 2 * l - temp2;
        tempr = h + 1.0 / 3.0;
        if (tempr > 1.0) tempr--;
        tempg = h;
        tempb = h - 1.0 / 3.0;
        if (tempb < 0.0) tempb++;

        //red
        if (tempr < 1.0 / 6.0) r = temp1 + (temp2 - temp1) * 6.0 * tempr;
        else if (tempr < 0.5) r = temp2;
        else if (tempr < 2.0 / 3.0) r = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempr) * 6.0;
        else r = temp1;

        //green
        if (tempg < 1.0 / 6.0) g = temp1 + (temp2 - temp1) * 6.0 * tempg;
        else if (tempg < 0.5) g = temp2;
        else if (tempg < 2.0 / 3.0) g = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempg) * 6.0;
        else g = temp1;

        //blue
        if (tempb < 1.0 / 6.0) b = temp1 + (temp2 - temp1) * 6.0 * tempb;
        else if (tempb < 0.5) b = temp2;
        else if (tempb < 2.0 / 3.0) b = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempb) * 6.0;
        else b = temp1;
    }

    return Adafruit_NeoPixel::Color(r * 255, g * 255, b * 255);
}

void FireDisplayPattern::GeneratePallet()
{
    //generate the palette
    for(int x = 0; x < 256; x++)
    {
        //HSLtoRGB is used to generate colors:
        //Hue goes from 0 to 85: red to yellow
        //Saturation is always the maximum: 255
        //Lightness is 0..255 for x=0..128, and 255 for x=128..255
        //set the palette to the calculated RGB value
        //m_heatColors[x] = HSLtoRGB(x / 3, 255, std::min(255, x * 2));

        uint16_t hue = 0;
        byte saturation = 255;
        byte value = 255;
    
        if (x < 128)
        {
            // bottom half of temperature range:
            // hue = 0 (red), value scaled from 0 to 255
            value = x * 2;    
        }
        else
        {
            // top half of temperature range:
            // hue from 0 to 8200 (yellowish), value at 255
            hue = (x - 128) / 255.0 * 8200;
        }
    
        m_heatColors[x] = Adafruit_NeoPixel::ColorHSV(hue, saturation, value);
    }
}