#include "TwoColorPattern.h"

TwoColorPattern::TwoColorPattern(ulong color1, ulong color2)
{
    m_color1 = color1;
    m_color2 = color2;

    m_colorCount = 2;
}

void TwoColorPattern::reset()
{
    m_iterationCount = 0;
}

ulong TwoColorPattern::getNextColor()
{
    int modulus = m_color1Duration + m_color2Duration;
    int remainder = m_iterationCount % modulus;
    ulong color = m_color2;
    if (remainder < m_color1Duration)
    {
        color = m_color1;
    }

    m_iterationCount++;

    return color;
}

uint TwoColorPattern::getNumberOfParameters()
{
    return getParameterNames().size();
}

void TwoColorPattern::setColor1Duration(byte duration)
{
    m_color1Duration = convertDuration(duration);
}

void TwoColorPattern::setColor2Duration(byte duration)
{
    m_color2Duration = convertDuration(duration);
}

byte TwoColorPattern::convertDuration(byte duration)
{
    // convert the 0-255 duration value to something more reasonable like 1-50
    int convertedDuration = MathUtils::rescaleInput(1, 50, duration);
    if (convertedDuration < 1)
    {
        convertedDuration = 1;
    }

    return convertedDuration;
}

void TwoColorPattern::incrementOnly(uint incrementAmount)
{
    m_iterationCount = incrementAmount;
}

std::vector<String> TwoColorPattern::getParameterNames()
{
    std::vector<String> parameterNames;
    parameterNames.push_back("Color 1 duration");
    parameterNames.push_back("Color 2 duration");

    return parameterNames;
}