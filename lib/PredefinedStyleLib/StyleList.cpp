#include "StyleList.h"

StyleList::StyleList(uint numberOfLists)
{
    for (uint i = 0; i < numberOfLists; i++)
    {
        m_styleLists.push_back(std::vector<StyleDefinition>());
    }
}

void StyleList::addStyleToList(uint listNumber, const StyleDefinition& style)
{
    if (listNumber >= m_styleLists.size())
    {
        return;
    }

    m_styleLists[listNumber].push_back(style);
}

StyleDefinition StyleList::getStyle(uint listNumber, uint sequenceNumber)
{
    if (listNumber >= m_styleLists.size()
        || m_styleLists[listNumber].size() == 0)
    {
        // Either the button number doesn't exist or there are no sequences defined.
        // Return a default style
        return StyleDefinitionBuilder::SolidColor(Colors::Pink);
    }
    
    int index = sequenceNumber % m_styleLists[listNumber].size();

    return m_styleLists[listNumber][index];
}
