#ifndef STYLECONFIGURATION_H
#define STYLECONFIGURATION_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include "Colors.h"
#include "StyleDefinition.h"

class StyleConfiguration
{
    public:
        static StyleConfiguration* ParseJson(const char* jsonString, size_t length);

        StyleDefinition& getDefaultStyle() { return m_defaultStyle; };
        std::vector<StyleDefinition>& getStyles() { return m_styles; };

    private:
        StyleConfiguration();
        // returns true if the style entry was successfully parsed, false otherwise.
        static bool tryParseStyleEntry(JsonVariant style, StyleDefinition& styleDefinition);
        // returns the style to use as the default if the named default style is not present in the list.
        static StyleDefinition getUnknownDefaultStyle();
        static void debugPrint(const char* message);
        static void debugPrintln(const char* message);

        StyleDefinition m_defaultStyle;
        std::vector<StyleDefinition> m_styles;
};

#endif
