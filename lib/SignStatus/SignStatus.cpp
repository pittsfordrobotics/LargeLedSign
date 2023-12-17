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
    this->brightness = other.brightness;
    this->patternNames = other.patternNames;
    this->speed = other.speed;
    this->styleNames = other.styleNames;
    this->signConfigurationData = other.signConfigurationData;
    this->patternData = other.patternData;
}

bool SignStatus::operator==(const SignStatus &other)
{
    return (
        this->brightness == other.brightness 
        && this->speed == other.speed 
        && this->styleNames.equals(other.styleNames) 
        && this->patternNames.equals(other.patternNames) 
        && this->signConfigurationData == other.signConfigurationData
        && this->patternData == other.patternData);
}

bool SignStatus::operator!=(const SignStatus &other)
{
    return (
        this->brightness != other.brightness 
        || this->speed != other.speed 
        || !this->styleNames.equals(other.styleNames)
        || !this->patternNames.equals(other.patternNames)
        || this->signConfigurationData != other.signConfigurationData
        || this->patternData != other.patternData);
}