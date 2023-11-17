#include "ServiceStatus.h"

ServiceStatus::ServiceStatus() {
}

ServiceStatus::ServiceStatus(const ServiceStatus& other) {
    copy(other);
}

void ServiceStatus::setSignData(String signData) {
    m_signData = signData;
    std::vector<String> splitSignData = StringUtils::splitString(m_signData, ';');

    if (splitSignData.size() < 4) {
        Serial.println("Too little data found for the sign data!");
        return;
    }

    m_signType = splitSignData.at(0).toInt();
    m_signOrder = splitSignData.at(1).toInt();
    m_columnCount = splitSignData.at(2).toInt();
    m_pixelCount = splitSignData.at(3).toInt();
}

ServiceStatus& ServiceStatus::operator=(const ServiceStatus& other) {
    copy(other);
    return *this;
}

void ServiceStatus::copy(const ServiceStatus& other) {
    m_brightness = other.m_brightness;
    m_pattern = other.m_pattern;
    m_patternNames = other.m_patternNames;
    m_speed = other.m_speed;
    m_step = other.m_step;
    m_style = other.m_style;
    m_styleNames = other.m_styleNames;
    m_signData = other.m_signData;
    m_signType = other.m_signType;
    m_signOrder = other.m_signOrder;
    m_columnCount = other.m_columnCount;
    m_pixelCount = other.m_pixelCount;
}

bool ServiceStatus::operator==(const ServiceStatus& other) {
    return (
        m_brightness == other.m_brightness
        && m_pattern == other.m_pattern
        && m_speed == other.m_speed
        && m_step == other.m_step
        && m_style == other.m_style
        && m_styleNames.equals(other.m_styleNames)
        && m_patternNames.equals(other.m_patternNames)
        && m_signData.equals(other.m_signData));
}