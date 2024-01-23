#include "PredefinedStyleList.h"

PredefinedStyleList::PredefinedStyleList(uint numberOfLists)
{
    for (uint i = 0; i < numberOfLists; i++)
    {
        m_styleLists.push_back(std::vector<PredefinedStyles>());
    }
}

void PredefinedStyleList::addStyleToList(uint listNumber, PredefinedStyles style)
{
    if (listNumber >= m_styleLists.size())
    {
        return;
    }

    m_styleLists[listNumber].push_back(style);
}

PredefinedStyle PredefinedStyleList::getStyle(uint listNumber, uint sequenceNumber)
{
    if (listNumber >= m_styleLists.size()
        || m_styleLists[listNumber].size() == 0)
    {
        // Either the button number doesn't exist or there are no sequences defined.
        return PredefinedStyle::getPredefinedStyle(m_defaultStyle);
    }
    
    int index = sequenceNumber % m_styleLists[listNumber].size();

    return PredefinedStyle::getPredefinedStyle(m_styleLists[listNumber][index]);
}
