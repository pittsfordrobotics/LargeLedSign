#ifndef SIGNOFFSETDATA_H
#define SIGNOFFSETDATA_H

#include <Arduino.h>
#include <vector>
#include <StringUtils.h>

class SignOffsetData {
    public:
        SignOffsetData();
        SignOffsetData(String offsetData);
        SignOffsetData(const SignOffsetData& other);
        SignOffsetData& operator=(const SignOffsetData& other);
        bool operator==(const SignOffsetData& other);
        bool operator!=(const SignOffsetData& other);

        int getDigitsToRight() { return m_digitsToRight; }
        int getDigitsToLeft() { return m_digitsToLeft; }
        int getColumnsToRight() { return m_columnsToRight; }
        int getColumnsToLeft() { return m_columnsToLeft; }

        void setDigitsToLeft(int digitsToLeft) { m_digitsToLeft = digitsToLeft; }
        void setDigitsToRight(int digitsToRight) { m_digitsToRight = digitsToRight; }
        void setColumnsToLeft(int columnsToLeft) { m_columnsToLeft = columnsToLeft; }
        void setColumnsToRight(int columnsToRight) { m_columnsToRight = columnsToRight; }

        String getOffsetDataString();

    private:
        int m_digitsToRight{-1};
        int m_digitsToLeft{-1};
        int m_columnsToRight{-1};
        int m_columnsToLeft{-1};

        void copy(const SignOffsetData& other);
};

#endif