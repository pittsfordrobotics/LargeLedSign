#ifndef STYLECONFIGURATION_H
#define STYLECONFIGURATION_H

#ifdef PIO_UNIT_TESTING
#include <ArduinoFake.h>
#else
#include <Arduino.h>
#endif
#include <ArduinoJson.h>
#include <vector>
#include "Colors.h"
#include "Patterns\PatternData.h"
#include "StyleDefinition.h"
#include "Utils\DebugUtils.h"

class StyleConfiguration
{
    public:
        static StyleConfiguration* ParseJson(const char* jsonString, size_t length);
        ~StyleConfiguration() = default;  // No dynamic memory to clean up

        StyleDefinition& getDefaultStyle() { return m_defaultStyle; };
        std::vector<StyleDefinition>& getStyles() { return m_styles; };

    private:
        StyleConfiguration();
        // returns true if the style entry was successfully parsed, false otherwise.
        static bool tryParseStyleEntry(JsonVariant style, StyleDefinition& styleDefinition);
        // returns the style to use as the default if the named default style is not present in the list.
        static StyleDefinition getUnknownDefaultStyle();

        StyleDefinition m_defaultStyle;
        std::vector<StyleDefinition> m_styles;
};

#endif
