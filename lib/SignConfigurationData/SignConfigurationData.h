#ifndef SIGNCONFIGURATIONDATA_H
#define SIGNCONFIGURATIONDATA_H

#include <Arduino.h>
#include <vector>
#include <StringUtils.h>

class SignConfigurationData {
    public:
        SignConfigurationData();
        SignConfigurationData(String signData);
        SignConfigurationData(const SignConfigurationData& other);
        SignConfigurationData& operator=(const SignConfigurationData& other);
        bool operator==(const SignConfigurationData& other);
        bool operator!=(const SignConfigurationData& other);

        int getSignType() { return m_signType; }
        int getSignOrder() { return m_signOrder; }
        int getColumnCount() { return m_columnCount; }
        int getPixelCount() { return m_pixelCount; }

        void setSignType(int signType) { m_signType = signType; }
        void setSignOrder(int signOrder) { m_signOrder = signOrder; }
        void setColumnCount(int columnCount) { m_columnCount = columnCount; }
        void setPixelCount(int pixelCount) { m_pixelCount = pixelCount; }

        String getConfigurationString();

    private:
        int m_signType{-1};
        int m_signOrder{-1};
        int m_columnCount{-1};
        int m_pixelCount{-1};

        void copy(const SignConfigurationData& other);
};

#endif