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
        int getDigitsToRight() { return m_digitsToRight; }
        int getDigitsToLeft() { return m_digitsToLeft; }
        int getColumnsToRight() { return m_columnsToRight; }
        int getColumnsToLeft() { return m_columnsToLeft; }
        int getPixelsToRight() { return m_pixelsToRight; }
        int getPixelsToLeft() { return m_pixelsToLeft; }

        void setSignType(int signType) { m_signType = signType; }
        void setSignOrder(int signOrder) { m_signOrder = signOrder; }
        void setColumnCount(int columnCount) { m_columnCount = columnCount; }
        void setPixelCount(int pixelCount) { m_pixelCount = pixelCount; }
        void setDigitsToLeft(int digitsToLeft) { m_digitsToLeft = digitsToLeft; }
        void setDigitsToRight(int digitsToRight) { m_digitsToRight = digitsToRight; }
        void setColumnsToLeft(int columnsToLeft) { m_columnsToLeft = columnsToLeft; }
        void setColumnsToRight(int columnsToRight) { m_columnsToRight = columnsToRight; }
        void setPixelsToLeft(int pixelsToLeft) { m_pixelsToLeft = pixelsToLeft; }
        void setPixelsToRight(int pixelsToRight) { m_pixelsToRight = pixelsToRight; }

        String getConfigurationString();

    private:
        int m_signType{-1};
        int m_signOrder{-1};
        int m_columnCount{-1};
        int m_pixelCount{-1};
        int m_digitsToRight{-1};
        int m_digitsToLeft{-1};
        int m_columnsToRight{-1};
        int m_columnsToLeft{-1};
        int m_pixelsToRight{-1};
        int m_pixelsToLeft{-1};

        void copy(const SignConfigurationData& other);
};

#endif