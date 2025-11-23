//
// Temporary class to return a default style configuration
// until we can read the style configuration from a JSON file.
//
#ifndef STYLECONFIGFACTORY_H
#define STYLECONFIGFACTORY_H

#include "StyleConfiguration.h"

class StyleConfigFactory
{
    public:
        static StyleConfiguration* createDefaultStyleConfiguration();
};

#endif