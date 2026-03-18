#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <Arduino.h>

class StringUtils
{
    public:
        static const char* copyString(const char* source, size_t length);
};

#endif // STRING_UTILS_H