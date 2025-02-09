#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

#include <Arduino.h>

// This class is used to provide debug output.
// Unit tests can't use the 'Serial' object, so this
// class emits debug output to the console when running
// unit tests.
class DebugUtils
{
    public:
        static void print(const char* message);
        static void print(String message) { print(message.c_str()); }
        static void println(const char* message);
        static void println(String message) { println(message.c_str()); }
};

#endif