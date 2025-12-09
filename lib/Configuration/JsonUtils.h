#ifndef JSONUTILS_H
#define JSONUTILS_H

#include <Arduino.h>
#include <ArduinoJson.h>

class JsonUtils 
{
    public:
        // General template for most types
        template <typename T>
        static T getValueOrDefault(JsonVariant jsonVariant, const char* nodeName, const T& defaultValue)
        {
            if (jsonVariant[nodeName].is<JsonVariant>())
            {
                return jsonVariant[nodeName].as<T>();
            }
            
            return defaultValue;
        }
};

// Template specialization for String type
template <>
inline String JsonUtils::getValueOrDefault<String>(JsonVariant jsonVariant, const char* nodeName, const String& defaultValue)
{
    if (jsonVariant[nodeName].is<JsonVariant>())
    {
        std::string valueAsStdStr = jsonVariant[nodeName].as<std::string>();
        return String(valueAsStdStr.c_str());
    }

    return defaultValue;
}

#endif