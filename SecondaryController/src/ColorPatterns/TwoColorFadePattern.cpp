#include "TwoColorFadePattern.h"

TwoColorFadePattern::TwoColorFadePattern(ulong color1, ulong color2)
{
    m_color1 = color1;
    m_color2 = color2;

    m_colorCount = 2;
}

double TwoColorFadePattern::applyBrightnessGamma(double inputBrightness) {
        byte inputScale = 255 * inputBrightness;
        byte outputScale = Adafruit_NeoPixel::gamma8(inputScale);
        
        return (double)outputScale / 255.0;
}

void TwoColorFadePattern::reset()
{
    m_iterationCount = 0;
    
    // Start with showing color 1
    for (int i = 0; i < m_colorDuration; i++)
    {
        m_colorSequence.push_back(m_color1);
    }

    // Fade out from color 1 to black.
    byte color1Red = (m_color1 & 0xFF0000) >> 16;
    byte color1Green = (m_color1 & 0xFF00) >> 8;
    byte color1Blue = (m_color1 & 0xFF);
    for (int i = m_fadeOutDuration - 1; i >=0; i--)
    {
        double scaling = applyBrightnessGamma((double)(i + 1)/(m_fadeOutDuration + 1));
        ulong color = Adafruit_NeoPixel::Color(color1Red * scaling, color1Green * scaling, color1Blue * scaling);
        m_colorSequence.push_back(color);
    }

    // Show black
    for (int i = 0; i < m_fadedDuration; i++)
    {
        m_colorSequence.push_back(0);
    }

    // Fade from black into color 2
    byte color2Red = (m_color2 & 0xFF0000) >> 16;
    byte color2Green = (m_color2 & 0xFF00) >> 8;
    byte color2Blue = (m_color2 & 0xFF);
    for (int i = 0; i < m_fadeInDuration; i++)
    {
        double scaling =  applyBrightnessGamma((double)(i + 1)/(m_fadeInDuration + 1));
        ulong color = Adafruit_NeoPixel::Color(color2Red * scaling, color2Green * scaling, color2Blue * scaling);
        m_colorSequence.push_back(color);
    }

    // Show color 2
    for (int i = 0; i < m_colorDuration; i++)
    {
        m_colorSequence.push_back(m_color2);
    }

    // Fade out from color 2 to black.
    for (int i = m_fadeOutDuration -1; i >= 0; i--)
    {
        double scaling =  applyBrightnessGamma((double)(i + 1)/(m_fadeOutDuration + 1));
        ulong color = Adafruit_NeoPixel::Color(color2Red * scaling, color2Green * scaling, color2Blue * scaling);
        m_colorSequence.push_back(color);
    }

    // Show black
    for (int i = 0; i < m_fadedDuration; i++)
    {
        m_colorSequence.push_back(0);
    }

    // Fade from black back into color 1.
    for (int i = 0; i < m_fadeInDuration; i++)
    {
        double scaling =  applyBrightnessGamma((double)(i + 1)/(m_fadeInDuration + 1));
        ulong color = Adafruit_NeoPixel::Color(color1Red * scaling, color1Green * scaling, color1Blue * scaling);
        m_colorSequence.push_back(color);
    }
}

ulong TwoColorFadePattern::getNextColor()
{
    // Check the size of the sequence list just in case.
    if (m_colorSequence.size() == 0)
    {
        return 0;
    }

    m_iterationCount = m_iterationCount % m_colorSequence.size();
    ulong color = m_colorSequence[m_iterationCount];
    m_iterationCount++;
    return color;
}

uint TwoColorFadePattern::getNumberOfParameters()
{
    return getParameterNames().size();
}

void TwoColorFadePattern::setColorDuration(byte duration)
{
    m_colorDuration = convertColorDuration(duration);
}

void TwoColorFadePattern::setFadeInDuration(byte duration)
{
    m_fadeInDuration = convertFadeInOutDuration(duration);
}

void TwoColorFadePattern::setFadeOutDuration(byte duration)
{
    m_fadeOutDuration = convertFadeInOutDuration(duration);
}

void TwoColorFadePattern::setFadedDuration(byte duration)
{
    m_fadedDuration = convertColorDuration(duration);
}

byte TwoColorFadePattern::convertColorDuration(byte duration)
{
    int convertedDuration = MathUtils::rescaleInput(1, 50, duration);
    if (convertedDuration < 1)
    {
        convertedDuration = 1;
    }

    return convertedDuration;
}

byte TwoColorFadePattern::convertFadeInOutDuration(byte duration)
{
    int convertedDuration = MathUtils::rescaleInput(0, 50, duration);
    if (convertedDuration < 0)
    {
        convertedDuration = 0;
    }

    return convertedDuration;
}

void TwoColorFadePattern::incrementOnly(uint incrementAmount)
{
    m_iterationCount = incrementAmount;
}

std::vector<String> TwoColorFadePattern::getParameterNames()
{
    std::vector<String> parameterNames;
    parameterNames.push_back("Color duration");
    parameterNames.push_back("Fade in duration");
    parameterNames.push_back("Fade out duration");
    parameterNames.push_back("Faded duration");

    return parameterNames;
}