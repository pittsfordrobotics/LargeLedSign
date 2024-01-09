#include "SignConfigurationData.h"

SignConfigurationData::SignConfigurationData()
{
}

SignConfigurationData::SignConfigurationData(const SignConfigurationData &other)
{
    copy(other);
}

void SignConfigurationData::copy(const SignConfigurationData &other)
{
    this->columnCount = other.columnCount;
    this->pixelCount = other.pixelCount;
    this->signOrder = other.signOrder;
    this->signType = other.signType;
}

SignConfigurationData &SignConfigurationData::operator=(const SignConfigurationData &other)
{
    copy(other);
    return *this;
}

bool SignConfigurationData::operator==(const SignConfigurationData &other)
{
    return (this->columnCount == other.columnCount 
        && this->pixelCount == other.pixelCount
        && this->signOrder == other.signOrder
        && this->signType == other.signType);
}

bool SignConfigurationData::operator!=(const SignConfigurationData &other)
{
    return (this->columnCount != other.columnCount
        || this->pixelCount != other.pixelCount
        || this->signOrder != other.signOrder 
        || this->signType != other.signType);
}