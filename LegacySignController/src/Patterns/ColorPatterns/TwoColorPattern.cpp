#include "TwoColorPattern.h"

TwoColorPattern::TwoColorPattern(uint32_t color1, uint32_t color2)
{
    m_color1 = color1;
    m_color2 = color2;

    m_colorCount = 2;
}

void TwoColorPattern::reset()
{
    m_iterationCount = 0;
    
    for (int i = 0; i < m_color1Duration; i++)
    {
        m_colorSequence.push_back(m_color1);
    }
    for (int i = 0; i < m_color2Duration; i++)
    {
        m_colorSequence.push_back(m_color2);
    }
}

uint32_t TwoColorPattern::getNextColor()
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

uint16_t TwoColorPattern::getNumberOfParameters()
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

void TwoColorPattern::incrementOnly(uint16_t incrementAmount)
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