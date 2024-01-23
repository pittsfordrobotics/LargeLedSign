#include "PredefinedStyleList.h"

PredefinedStyleList::PredefinedStyleList(uint numberOfLists)
{
    for (uint i = 0; i < numberOfLists; i++)
    {
        m_buttonStyles.push_back(std::vector<PredefinedStyles>());
    }
}

void PredefinedStyleList::addStyleToList(uint listNumber, PredefinedStyles style)
{
    if (listNumber >= m_buttonStyles.size())
    {
        return;
    }

    m_buttonStyles[listNumber].push_back(style);
}

PredefinedStyle PredefinedStyleList::getStyleForButton(uint buttonNumber, uint sequenceNumber)
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
