#ifndef STYLECONFIGURATION_H
#define STYLECONFIGURATION_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include "StyleDefinition.h"

class StyleConfiguration
{
    public:
        static StyleConfiguration ParseJson(const char* jsonString, size_t length);

        String getDefaultStyleName() { return m_defaultStyleName; };
        std::vector<StyleDefinition>& getStyles() { return m_styles; };

    private:
        StyleConfiguration();
        // returns true if the style entry was successfully parsed, false otherwise.
        static bool tryParseStyleEntry(JsonVariant style, StyleDefinition& styleDefinition);
        static void debugPrint(const char* message);
        static void debugPrintln(const char* message);

        String m_defaultStyleName{""};
        std::vector<StyleDefinition> m_styles;
};

#endif
