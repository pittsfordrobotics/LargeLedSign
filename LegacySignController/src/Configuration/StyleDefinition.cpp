#include "StyleDefinition.h"
StyleDefinition::StyleDefinition()
{
}

StyleDefinition::StyleDefinition(PatternData patternData, byte speed)
{
    m_patternData = patternData;
    m_speed = speed;
}

StyleDefinition::StyleDefinition(const StyleDefinition& other)
{
    this->m_patternData = other.m_patternData;
    this->m_speed = other.m_speed;
    this->m_name = other.m_name;
}

// Assignment operator
StyleDefinition& StyleDefinition::operator=(const StyleDefinition& other)
{
    this->m_patternData = other.m_patternData;
    this->m_speed = other.m_speed;
    this->m_name = other.m_name;
    return *this;
}

bool StyleDefinition::operator==(const StyleDefinition& other)
{
    // Ignoring "name" field for comparison
    return (this->m_patternData == other.m_patternData
        && this->m_speed == other.m_speed);
}

bool StyleDefinition::operator!=(const StyleDefinition& other)
{
    // Ignoring "name" field for comparison
    return (this->m_patternData != other.m_patternData
        || this->m_speed != other.m_speed);
}
