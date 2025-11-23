#include "ColorFadePattern.h"

ColorFadePattern::ColorFadePattern(uint32_t color1, uint32_t color2)
{
    m_colors.push_back(color1);
    m_colors.push_back(color2);

    m_colorCount = 2;
}

ColorFadePattern::ColorFadePattern(uint32_t color1, uint32_t color2, uint32_t color3)
{
    m_colors.push_back(color1);
    m_colors.push_back(color2);
    m_colors.push_back(color3);

    m_colorCount = 3;
}

ColorFadePattern::ColorFadePattern(uint32_t color1, uint32_t color2, uint32_t color3, uint32_t color4)
{
    m_colors.push_back(color1);
    m_colors.push_back(color2);
    m_colors.push_back(color3);
    m_colors.push_back(color4);

    m_colorCount = 4;
}

double ColorFadePattern::applyBrightnessGamma(double inputBrightness) {
        byte inputScale = 255 * inputBrightness;
        byte outputScale = Adafruit_NeoPixel::gamma8(inputScale);
        
        return (double)outputScale / 255.0;
}

void ColorFadePattern::reset()
{
    m_iterationCount = 0;
    
    for (uint16_t i = 0; i < m_colors.size(); i++)
    {
        // Fade in
        uint32_t targetColor = m_colors[i];
        byte colorRed = (targetColor & 0xFF0000) >> 16;
        byte colorGreen = (targetColor & 0xFF00) >> 8;
        byte colorBlue = (targetColor & 0xFF);
        for (int i = 0; i < m_fadeInDuration; i++)
        {
            double scaling =  applyBrightnessGamma((double)(i + 1)/(m_fadeInDuration + 1));
            uint32_t color = Adafruit_NeoPixel::Color(colorRed * scaling, colorGreen * scaling, colorBlue * scaling);
            m_colorSequence.push_back(color);
        }

        // Show the color
        for (int i = 0; i < m_colorDuration; i++)
        {
            m_colorSequence.push_back(targetColor);
        }

        // Fade out
        for (int i = m_fadeOutDuration -1; i >= 0; i--)
        {
            double scaling =  applyBrightnessGamma((double)(i + 1)/(m_fadeOutDuration + 1));
            uint32_t color = Adafruit_NeoPixel::Color(colorRed * scaling, colorGreen * scaling, colorBlue * scaling);
            m_colorSequence.push_back(color);
        }

        // Show black
        for (int i = 0; i < m_fadedDuration; i++)
        {
            m_colorSequence.push_back(0);
        }
    }
}

ulong ColorFadePattern::getNextColor()
{
    // Check the size of the sequence list just in case.
    if (m_colorSequence.size() == 0)
    {
        return 0;
    }

    m_iterationCount = m_iterationCount % m_colorSequence.size();
    uint32_t color = m_colorSequence[m_iterationCount];
    m_iterationCount++;
    return color;
}

void ColorFadePattern::setColorDuration(byte duration)
{
    m_colorDuration = convertColorDuration(duration);

    // Make sure we display the color for at least 1 cycle.
    if (m_colorDuration < 1)
    {
        m_colorDuration = 1;
    }
}

void ColorFadePattern::setFadeInDuration(byte duration)
{
    m_fadeInDuration = convertFadeInOutDuration(duration);
}

void ColorFadePattern::setFadeOutDuration(byte duration)
{
    m_fadeOutDuration = convertFadeInOutDuration(duration);
}

void ColorFadePattern::setFadedDuration(byte duration)
{
    m_fadedDuration = convertColorDuration(duration);
}

byte ColorFadePattern::convertColorDuration(byte duration)
{
    int convertedDuration = MathUtils::rescaleInput(0, 50, duration);
    if (convertedDuration < 0)
    {
        convertedDuration = 0;
    }

    return convertedDuration;
}

byte ColorFadePattern::convertFadeInOutDuration(byte duration)
{
    int convertedDuration = MathUtils::rescaleInput(0, 50, duration);
    if (convertedDuration < 0)
    {
        convertedDuration = 0;
    }

    return convertedDuration;
}

void ColorFadePattern::incrementOnly(uint16_t incrementAmount)
{
    m_iterationCount = incrementAmount;
}

uint16_t ColorFadePattern::getNumberOfParameters()
{
    return getParameterNames().size();
}

std::vector<String> ColorFadePattern::getParameterNames()
{
    std::vector<String> parameterNames;
    parameterNames.push_back("Color duration");
    parameterNames.push_back("Fade in duration");
    parameterNames.push_back("Fade out duration");
    parameterNames.push_back("Faded duration");

    return parameterNames;
}