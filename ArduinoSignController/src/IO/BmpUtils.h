#ifndef BMP_UTILS_H
#define BMP_UTILS_H

#include <Arduino.h>
#include <SD.h>
#include <cstddef>
#include <vector>

class BmpUtils
{
    public:
        static std::vector<std::vector<unsigned long>> ParseBmp(File& file);
        static std::vector<std::vector<unsigned long>> ParseBmp(const std::vector<uint8_t>& bytes);
        static std::vector<std::vector<unsigned long>> ParseBmp(const uint8_t* bytes, size_t length);
};

#endif // BMP_UTILS_H
