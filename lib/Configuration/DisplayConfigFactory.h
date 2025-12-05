#ifndef DISPLAYCONFIGFACTORY_H
#define DISPLAYCONFIGFACTORY_H

// Temporary factory class to create DisplayConfiguration instances
// until the SD Card reader logic is implemented.

#include "DisplayConfiguration.h"

class DisplayConfigFactory
{
    public:
        static const char* getDigit1Json();
        static const char* getDigit3Json();
        static const char* getDigit8Json();
        static const char* getLogoJson();
        static const char* getTestMatrixJson();
        static std::vector<DisplayConfiguration>* createForTestMatrix();
        static std::vector<DisplayConfiguration>* createForLegacySign();
        static std::vector<DisplayConfiguration>* createForDigit1();
        static std::vector<DisplayConfiguration>* createForDigit3();
        static std::vector<DisplayConfiguration>* createForDigit8();
};

#endif // DISPLAYCONFIGFACTORY_H