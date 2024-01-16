#include "ButtonConfiguration.h"

ButtonConfiguration::ButtonConfiguration()
{
    // Styles for button 1 (id 0)
    m_buttonStyles.push_back(std::vector<PredefinedStyles>
    {
        PredefinedStyles::Pink_Solid
    });

    // Styles for button 2 (id 1)
    m_buttonStyles.push_back(std::vector<PredefinedStyles>
    {
        PredefinedStyles::RedPink_Right,
        PredefinedStyles::RedPink_CenterOut
    });

    // Styles for button 3 (id 2)
    m_buttonStyles.push_back(std::vector<PredefinedStyles>
    {
        PredefinedStyles::BluePink_Right,
        PredefinedStyles::BluePink_CenterOut
    });

    // Styles for button 4 (id 3)
    m_buttonStyles.push_back(std::vector<PredefinedStyles>
    {
        PredefinedStyles::Rainbow_Right,
        PredefinedStyles::Rainbow_Random
    });

    // Not bothering with alternate styles yet, but the configuration would be pretty much the same.
}

PredefinedStyle ButtonConfiguration::getStyleForButton(uint buttonNumber, uint sequenceNumber)
{
    if (buttonNumber >= m_buttonStyles.size()
        || m_buttonStyles[buttonNumber].size() == 0)
    {
        // Either the button number doesn't exist or there are no sequences defined.
        return PredefinedStyle::getPredefinedStyle(m_defaultStyle);
    }
    
    int index = sequenceNumber % m_buttonStyles[buttonNumber].size();

    return PredefinedStyle::getPredefinedStyle(m_buttonStyles[buttonNumber][index]);
}

PredefinedStyle ButtonConfiguration::getAlternateStyleForButton(uint buttonNumber, uint sequenceNumber)
{
    if (buttonNumber >= m_alternateButtonStyles.size()
        || m_alternateButtonStyles[buttonNumber].size() == 0)
    {
        // Either the button number doesn't exist or there are no sequences defined.
        return PredefinedStyle::getPredefinedStyle(m_defaultStyle);
    }

    int index = sequenceNumber % m_alternateButtonStyles[buttonNumber].size();

    return PredefinedStyle::getPredefinedStyle(m_alternateButtonStyles[buttonNumber][index]);
}