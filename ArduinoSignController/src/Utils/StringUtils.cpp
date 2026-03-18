#include "StringUtils.h"

const char* StringUtils::copyString(const char* source, size_t length) 
{
    char* dest = new char[length + 1];
    strncpy(dest, source, length);
    dest[length] = '\0'; // Null-terminate the string
    return dest;
}