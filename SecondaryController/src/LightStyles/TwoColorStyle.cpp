#include "Arduino.h"
#include "TwoColorStyle.h"
#include "PixelBuffer.h"

TwoColorStyle::TwoColorStyle(String name, uint32_t color1, uint32_t color2, PixelBuffer *pixelBuffer) : LightStyle(name, pixelBuffer)
{
    m_color1 = color1;
    m_color2 = color2;
    m_iterationCount = 0;
    m_nextUpdate = 0;
}

TwoColorStyle::~TwoColorStyle()
{
}

void TwoColorStyle::update()
{
    if (millis() < m_nextUpdate)
    {
        return;
    }

    ulong primaryColor = m_color1;
    ulong secondaryColor = m_color2;
    int mod = getModulus();

    if (m_step > 50)
    {
        primaryColor = m_color2;
        secondaryColor = m_color1;
    }

    if (m_pattern == static_cast<int>(LightPatterns::Random))
    {
        // Make the percent a const or define
        double percentToFill = 10 / 100.0;
        // The ratio of secondary pixels compared to all pixels is 1/mod.
        double totalNumberToFill = m_pixelBuffer->getPixelCount() * percentToFill;
        int numberOfSecondaryPixels = totalNumberToFill / mod;
        int numberOfPrimaryPixels = totalNumberToFill - numberOfSecondaryPixels;
        m_pixelBuffer->fillRandomly(primaryColor, numberOfPrimaryPixels);
        m_pixelBuffer->fillRandomly(secondaryColor, numberOfSecondaryPixels);
    }
    else
    {
        updateSequentialPattern(primaryColor, secondaryColor, 1);
    }

    m_nextUpdate = millis() + getIterationDelay();
}

void TwoColorStyle::reset()
{
    m_iterationCount = 0;
    m_nextUpdate = 0;
    uint32_t primaryColor = m_color1;
    uint32_t secondaryColor = m_color2;

    if (m_step > 50)
    {
        primaryColor = m_color2;
        secondaryColor = m_color1;
    }

    if (m_pattern == static_cast<int>(LightPatterns::Random))
    {
        // Fill with the primary color
        m_pixelBuffer->fill(primaryColor);
    }
    else
    {
        m_iterationCount = getNumberOfBlocksToDrain();
        updateSequentialPattern(primaryColor, secondaryColor, getNumberOfBlocksForPattern());
    }
}

void TwoColorStyle::updateSequentialPattern(ulong primaryColor, ulong secondaryColor, int numberOfTimesToUpdate)
{
    int mod = getModulus();
    int maxModRemainder = 0;

    if (shouldPatternBeDoubled())
    {
        maxModRemainder = 1;
        if (mod < 4)
        {
            mod = 4;
        }
    }

    for (int i = 0; i < numberOfTimesToUpdate; i++)
    {
        ulong newColor = primaryColor;
        if (mod > 0 && m_iterationCount % mod <= maxModRemainder)
        {
            newColor = secondaryColor;
        }

        shiftColorUsingPattern(newColor);
        m_iterationCount++;
    }
}

int TwoColorStyle::getIterationDelay()
{
    // Convert "speed" to a delay.
    // Speed ranges from 1 to 100.
    int minDelay = 20;
    int maxDelay = 800;
    int minSpeed = 100; // Yes, reversed.  100 correlates to "fastest", ie, min delay
    int maxSpeed = 1;
    double m = (double)(maxDelay - minDelay) / (maxSpeed - minSpeed);
    double b = maxDelay - m;
    int delay = m_speed * m + b;
    return delay;
}

int TwoColorStyle::getModulus()
{
    // Convert "step" to a modulus -- every "modulus" pixel will be the secondary color.
    // Step ranges from 1 to 100.
    // The modulus will be the minumum (2) at 50, and increase as you go away from 50.
    int minMod = 2;
    int maxMod = 10;
    int minStep = 1;
    int maxStep = 100;
    double midStep = (double)(maxStep - minStep) / 2.0;
    int x = abs(m_step - midStep);
    // If we're within about 5 of the max/min, report no mod so it's a solid color.
    if (midStep - x < 5)
    {
        return -1;
    }
    double m = (double)(maxMod - minMod) / (midStep - minStep);
    double b = minMod;
    int mod = x * m + b;
    return mod;
}

// When shifting by rows or columns, a single row or column
// is too "thin" to be very visible, so double up the pattern
// and emit at least 2 rows or columns of the same color.
bool TwoColorStyle::shouldPatternBeDoubled()
{
    return m_pattern == static_cast<int>(LightPatterns::Right) 
        || m_pattern == static_cast<int>(LightPatterns::Left) 
        || m_pattern == static_cast<int>(LightPatterns::Up) 
        || m_pattern == static_cast<int>(LightPatterns::Down);
}
