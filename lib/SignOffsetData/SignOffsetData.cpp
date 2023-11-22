#include "SignOffsetData.h"

SignOffsetData::SignOffsetData() {
}

SignOffsetData::SignOffsetData(const SignOffsetData& other) {
    copy(other);
}

SignOffsetData::SignOffsetData(String offsetData) {
    std::vector<String> splitOffsetData = StringUtils::splitString(offsetData, ';');

    if (splitOffsetData.size() < 4) {
        Serial.println("Too little data found for the offset data!");
        return;
    }

    m_digitsToLeft = splitOffsetData[0].toInt();
    m_digitsToRight = splitOffsetData[1].toInt();
    m_columnsToLeft = splitOffsetData[2].toInt();
    m_columnsToRight = splitOffsetData[3].toInt();
}

String SignOffsetData::getOffsetDataString() {
    // digitsL;digitsR;columnsL;columnsR

    std::vector<String> strings;
    strings.push_back(String(m_digitsToLeft));
    strings.push_back(String(m_digitsToRight));
    strings.push_back(String(m_columnsToLeft));
    strings.push_back(String(m_columnsToRight));

    return StringUtils::joinStrings(strings, ';');
}

void SignOffsetData::copy(const SignOffsetData& other) {
    m_digitsToLeft = other.m_digitsToLeft;
    m_digitsToRight = other.m_digitsToRight;
    m_columnsToLeft = other.m_columnsToLeft;
    m_columnsToRight = other.m_columnsToRight;
}

SignOffsetData& SignOffsetData::operator=(const SignOffsetData& other) {
    copy(other);
    return *this;
}

bool SignOffsetData::operator==(const SignOffsetData& other) {
    return (
        m_digitsToLeft == other.m_digitsToLeft
        && m_digitsToRight == other.m_digitsToRight
        && m_columnsToLeft == other.m_columnsToLeft
        && m_columnsToRight == other.m_columnsToRight);
}

bool SignOffsetData::operator!=(const SignOffsetData& other) {
    return (
        m_digitsToLeft != other.m_digitsToLeft
        || m_digitsToRight != other.m_digitsToRight
        || m_columnsToLeft != other.m_columnsToLeft
        || m_columnsToRight != other.m_columnsToRight);
}