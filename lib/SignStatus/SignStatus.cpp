#include "SignStatus.h"

SignStatus::SignStatus()
{
}

SignStatus::SignStatus(const SignStatus &other)
{
    copy(other);
}

SignStatus &SignStatus::operator=(const SignStatus &other)
{
    copy(other);
    return *this;
}

void SignStatus::copy(const SignStatus &other)
{
    m_brightness = other.m_brightness;
    m_pattern = other.m_pattern;
    m_patternNames = other.m_patternNames;
    m_speed = other.m_speed;
    m_step = other.m_step;
    m_style = other.m_style;
    m_styleNames = other.m_styleNames;
    m_signConfigData = other.m_signConfigData;
    m_patternData = other.m_patternData;
}

bool SignStatus::operator==(const SignStatus &other)
{
    return (
        m_brightness == other.m_brightness 
        && m_pattern == other.m_pattern 
        && m_speed == other.m_speed 
        && m_step == other.m_step 
        && m_style == other.m_style 
        && m_styleNames.equals(other.m_styleNames) 
        && m_patternNames.equals(other.m_patternNames) 
        && m_signConfigData == other.m_signConfigData
        && m_patternData == other.m_patternData);
}

bool SignStatus::operator!=(const SignStatus &other)
{
    return (
        m_brightness != other.m_brightness 
        || m_pattern != other.m_pattern 
        || m_speed != other.m_speed 
        || m_step != other.m_step 
        || m_style != other.m_style 
        || !m_styleNames.equals(other.m_styleNames)
        || !m_patternNames.equals(other.m_patternNames)
        || m_signConfigData != other.m_signConfigData
        || m_patternData != other.m_patternData);
}