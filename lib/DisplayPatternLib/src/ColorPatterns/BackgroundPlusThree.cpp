#include "BackgroundPlusThree.h"

BackgroundPlusThree::BackgroundPlusThree(uint32_t background, uint32_t color1, uint32_t color2, uint32_t color3)
{
    m_backgroundColor = background;
    m_color1 = color1;
    m_color2 = color2;
    m_color3 = color3;

    m_colorCount = 4;
}

void BackgroundPlusThree::reset()
{
    m_iterationCount = 0;
    
    for (int i = 0; i < m_backgroundDuration; i++)
    {
        m_colorSequence.push_back(m_backgroundColor);
    }
    for (int i = 0; i < m_color1Duration; i++)
    {
        m_colorSequence.push_back(m_color1);
    }
    for (int i = 0; i < m_backgroundDuration; i++)
    {
        m_colorSequence.push_back(m_backgroundColor);
    }
    for (int i = 0; i < m_color2Duration; i++)
    {
        m_colorSequence.push_back(m_color2);
    }
    for (int i = 0; i < m_backgroundDuration; i++)
    {
        m_colorSequence.push_back(m_backgroundColor);
    }
    for (int i = 0; i < m_color3Duration; i++)
    {
        m_colorSequence.push_back(m_color3);
    }
}

ulong BackgroundPlusThree::getNextColor()
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

uint16_t BackgroundPlusThree::getNumberOfParameters()
{
    return getParameterNames().size();
}

void BackgroundPlusThree::setBackgroundDuration(byte duration)
{
    m_backgroundDuration = convertDuration(duration);
}

void BackgroundPlusThree::setColor1Duration(byte duration)
{
    m_color1Duration = convertDuration(duration);
}

void BackgroundPlusThree::setColor2Duration(byte duration)
{
    m_color2Duration = convertDuration(duration);
}

void BackgroundPlusThree::setColor3Duration(byte duration)
{
    m_color3Duration = convertDuration(duration);
}

byte BackgroundPlusThree::convertDuration(byte duration)
{
    // convert the 0-255 duration value to something more reasonable like 1-50
    int convertedDuration = MathUtils::rescaleInput(1, 50, duration);
    if (convertedDuration < 1)
    {
        convertedDuration = 1;
    }

    return convertedDuration;
}

void BackgroundPlusThree::incrementOnly(uint16_t incrementAmount)
{
    m_iterationCount = incrementAmount;
}

std::vector<String> BackgroundPlusThree::getParameterNames()
{
    std::vector<String> parameterNames;
    parameterNames.push_back("Background duration");
    parameterNames.push_back("Color 1 duration");
    parameterNames.push_back("Color 2 duration");
    parameterNames.push_back("Color 3 duration");

    return parameterNames;
}