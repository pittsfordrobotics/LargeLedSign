#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <Arduino.h>
#include <vector>

class StringUtils {
    public:
        static std::vector<String> splitString(String input, char delimiter);
        static String joinStrings(std::vector<String> strings, char delimiter);
};

#endif