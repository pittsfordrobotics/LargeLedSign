#include "SignConfigurationData.h"

SignConfigurationData::SignConfigurationData() {
}

SignConfigurationData::SignConfigurationData(const SignConfigurationData& other) {
    copy(other);
}

SignConfigurationData::SignConfigurationData(String signData) {
    std::vector<String> splitSignData = StringUtils::splitString(signData, ';');

    if (splitSignData.size() < 4) {
        Serial.println("Too little data found for the sign data!");
        return;
    }

    m_signType = splitSignData[0].toInt();
    m_signOrder = splitSignData[1].toInt();
    m_columnCount = splitSignData[2].toInt();
    m_pixelCount = splitSignData[3].toInt();

    if (splitSignData.size() >= 10) {
        m_digitsToLeft = splitSignData[4].toInt();
        m_digitsToRight = splitSignData[5].toInt();
        m_columnsToLeft = splitSignData[6].toInt();
        m_columnsToRight = splitSignData[7].toInt();
        m_pixelsToLeft = splitSignData[8].toInt();
        m_pixelsToRight = splitSignData[9].toInt();
    }
}

String SignConfigurationData::getConfigurationString() {
    // type;order;numberOfColumns;numberOfPixels;digitsL;digitsR;columnsL;columnsR;pixelsL;pixelsR

    std::vector<String> strings;
    strings.push_back(String(m_signType));
    strings.push_back(String(m_signOrder));
    strings.push_back(String(m_columnCount));
    strings.push_back(String(m_pixelCount));
    strings.push_back(String(m_digitsToLeft));
    strings.push_back(String(m_digitsToRight));
    strings.push_back(String(m_columnsToLeft));
    strings.push_back(String(m_columnsToRight));
    strings.push_back(String(m_pixelsToLeft));
    strings.push_back(String(m_pixelsToRight));

    return StringUtils::joinStrings(strings, ';');
}

void SignConfigurationData::copy(const SignConfigurationData& other) {
    m_columnCount = other.m_columnCount;
    m_pixelCount = other.m_pixelCount;
    m_signOrder = other.m_signOrder;
    m_signType = other.m_signType;
    m_digitsToLeft = other.m_digitsToLeft;
    m_digitsToRight = other.m_digitsToRight;
    m_columnsToLeft = other.m_columnsToLeft;
    m_columnsToRight = other.m_columnsToRight;
    m_pixelsToLeft = other.m_pixelsToLeft;
    m_pixelsToRight = other.m_pixelsToRight;
}

SignConfigurationData& SignConfigurationData::operator=(const SignConfigurationData& other) {
    copy(other);
    return *this;
}

bool SignConfigurationData::operator==(const SignConfigurationData& other) {
    return (
        m_columnCount == other.m_columnCount
        && m_pixelCount == other.m_pixelCount
        && m_signOrder == other.m_signOrder
        && m_signType == other.m_signType
        && m_digitsToLeft == other.m_digitsToLeft
        && m_digitsToRight == other.m_digitsToRight
        && m_columnsToLeft == other.m_columnsToLeft
        && m_columnsToRight == other.m_columnsToRight
        && m_pixelsToLeft == other.m_pixelsToLeft
        && m_pixelsToRight == other.m_pixelsToRight);
}

bool SignConfigurationData::operator!=(const SignConfigurationData& other) {
    return (
        m_columnCount != other.m_columnCount
        || m_pixelCount != other.m_columnCount
        || m_signOrder != other.m_signOrder
        || m_signType != other.m_signType
        || m_digitsToLeft != other.m_digitsToLeft
        || m_digitsToRight != other.m_digitsToRight
        || m_columnsToLeft != other.m_columnsToLeft
        || m_columnsToRight != other.m_columnsToRight
        || m_pixelsToLeft != other.m_pixelsToLeft
        || m_pixelsToRight != other.m_pixelsToRight);
}