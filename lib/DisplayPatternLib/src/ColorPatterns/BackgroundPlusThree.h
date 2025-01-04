#ifndef BACKGROUNDPLUSTHREE_H
#define BACKGROUNDPLUSTHREE_H

#include <Arduino.h>
#include "ColorPattern.h"
#include <MathUtils.h>
#include <vector>

class BackgroundPlusThree : public ColorPattern
{
    public:
        BackgroundPlusThree(unsigned long background, unsigned long color1, unsigned long color2, unsigned long color3);

        virtual void reset();
        virtual unsigned long getNextColor();
        virtual void incrementOnly(unsigned int incrementAmount);
        virtual unsigned int getNumberOfParameters();

        void setBackgroundDuration(byte duration);
        void setColor1Duration(byte duration);
        void setColor2Duration(byte duration);
        void setColor3Duration(byte duration);

        static std::vector<String> getParameterNames();

    private:
        unsigned long m_backgroundColor{0};
        unsigned long m_color1{0};
        unsigned long m_color2{0};
        unsigned long m_color3{0};
        byte m_backgroundDuration{1};
        byte m_color1Duration{1};
        byte m_color2Duration{1};
        byte m_color3Duration{1};
        unsigned long m_iterationCount{0};

        std::vector<unsigned long> m_colorSequence;
        byte convertDuration(byte duration);
};

#endif