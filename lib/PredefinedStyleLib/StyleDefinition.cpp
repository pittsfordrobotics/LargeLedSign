#include "StyleDefinition.h"

StyleDefinition::StyleDefinition(PatternData patternData, byte speed)
{
    m_patternData = patternData;
    m_speed = speed;
}

StyleDefinition::StyleDefinition(const StyleDefinition& other)
{
    this->m_patternData = other.m_patternData;
    this->m_speed = other.m_speed;
}

// Assignment operator
StyleDefinition& StyleDefinition::operator=(const StyleDefinition& other)
{
    this->m_patternData = other.m_patternData;
    this->m_speed = other.m_speed;
    return *this;
}

bool StyleDefinition::operator==(const StyleDefinition& other)
{
    return (this->m_patternData == other.m_patternData
        && this->m_speed == other.m_speed);
}

bool StyleDefinition::operator!=(const StyleDefinition& other)
{
    return (this->m_patternData != other.m_patternData
        || this->m_speed != other.m_speed);
}