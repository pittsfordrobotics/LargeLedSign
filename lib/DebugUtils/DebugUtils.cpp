#include "DebugUtils.h"

void DebugUtils::print(const char* message)
{
#ifdef PIO_UNIT_TESTING
    printf("%s", message);
#else
    Serial.print(message);
#endif      
}

void DebugUtils::println(const char* message)
{
#ifdef PIO_UNIT_TESTING
    printf("%s\n", message);
#else
    Serial.println(message);
#endif      
}