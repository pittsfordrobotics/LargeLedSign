#ifndef DISPLAYCONFIGFACTORY_H
#define DISPLAYCONFIGFACTORY_H

// Temporary factory class to create DisplayConfiguration instances
// until the SD Card reader logic is implemented.

#include "DisplayConfiguration.h"

class DisplayConfigFactory
{
    public:
        static std::vector<DisplayConfiguration>* createForTestMatrix();
        static std::vector<DisplayConfiguration>* createForLegacySign();
};

#endif // DISPLAYCONFIGFACTORY_H