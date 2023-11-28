#include "SignConfigurationData.h"

SignConfigurationData::SignConfigurationData()
{
}

SignConfigurationData::SignConfigurationData(const SignConfigurationData &other)
{
    copy(other);
}

SignConfigurationData::SignConfigurationData(String signData)
{
    std::vector<String> splitSignData = StringUtils::splitString(signData, ';');

    if (splitSignData.size() < 4)
    {
        return;
    }

    m_signType = splitSignData[0].toInt();
    m_signOrder = splitSignData[1].toInt();
    m_columnCount = splitSignData[2].toInt();
    m_pixelCount = splitSignData[3].toInt();
}

String SignConfigurationData::getConfigurationString()
{
    // type;order;numberOfColumns;numberOfPixels

    std::vector<String> strings;
    strings.push_back(String(m_signType));
    strings.push_back(String(m_signOrder));
    strings.push_back(String(m_columnCount));
    strings.push_back(String(m_pixelCount));

    return StringUtils::joinStrings(strings, ';');
}

void SignConfigurationData::copy(const SignConfigurationData &other)
{
    m_columnCount = other.m_columnCount;
    m_pixelCount = other.m_pixelCount;
    m_signOrder = other.m_signOrder;
    m_signType = other.m_signType;
}

SignConfigurationData &SignConfigurationData::operator=(const SignConfigurationData &other)
{
    copy(other);
    return *this;
}

bool SignConfigurationData::operator==(const SignConfigurationData &other)
{
    return (
        m_columnCount == other.m_columnCount && m_pixelCount == other.m_pixelCount && m_signOrder == other.m_signOrder && m_signType == other.m_signType);
}

bool SignConfigurationData::operator!=(const SignConfigurationData &other)
{
    return (
        m_columnCount != other.m_columnCount || m_pixelCount != other.m_columnCount || m_signOrder != other.m_signOrder || m_signType != other.m_signType);
}