#include "StyleConfiguration.h"

StyleConfiguration::StyleConfiguration()
{
}

StyleConfiguration* StyleConfiguration::ParseJson(const char* jsonString, size_t length)
{
    StyleConfiguration* styleConfiguration = new StyleConfiguration();
    styleConfiguration->m_defaultStyle = getUnknownDefaultStyle();

    JsonDocument configDoc;
    DeserializationError err = deserializeJson(configDoc, jsonString, length);
    
    if (err == DeserializationError::EmptyInput)
    {
        debugPrintln("Style configuration JSON string was empty.");
        return styleConfiguration;
    }

    if (err != DeserializationError::Ok)
    {
        debugPrint("Error parsing style configuration JSON: ");
        debugPrintln(err.c_str());
        return styleConfiguration;
    }

    if (configDoc.isNull())
    {
        debugPrintln("Style configuration JSON was null after parsing.");
        return styleConfiguration;
    }

    String defaultStyleName("");
    bool defaultStyleSet = false;
    
    if (configDoc["defaultStyleName"].is<JsonVariant>())
    {
        // For some reason, the as<String> method doesn't work here, so using the as<std::string> method instead.
        // I'm using the Arduino String class here since it's an easier class to use.
        std::string name = configDoc["defaultStyleName"].as<std::string>();
        defaultStyleName = String(name.c_str());
    }

    if (configDoc["predefinedStyles"].is<JsonArray>())
    {
        JsonArray styles = configDoc["predefinedStyles"].as<JsonArray>();
        for (JsonObject style : styles)
        {
            StyleDefinition styleDefinition;
            if (tryParseStyleEntry(style, styleDefinition))
            {
                styleConfiguration->m_styles.push_back(styleDefinition);
                
                // See if this style is supposed to be the default style.
                if (!defaultStyleSet || defaultStyleName.equalsIgnoreCase(styleDefinition.getName()))
                {
                    // Either it's the first valid style in the list, or it's the one named as the default.
                    styleConfiguration->m_defaultStyle = styleDefinition;
                    defaultStyleSet = true;
                }
            }
        }
    }

    return styleConfiguration;
}

bool StyleConfiguration::tryParseStyleEntry(JsonVariant style, StyleDefinition& styleDefinition)
{
    if (!style["name"].is<JsonVariant>())
    {
        debugPrintln("Style entry is missing a name.");
        return false;
    }

    String styleName = style["name"].as<std::string>().c_str();
    debugPrint("Parsing style with name ");
    debugPrintln(styleName.c_str());
    styleDefinition.setName(styleName);

    if (!style["colorPattern"].is<JsonVariant>())
    {
        debugPrintln("Style entry is missing a color pattern.");
        return false;
    }

    if (!style["displayPattern"].is<JsonVariant>())
    {
        debugPrintln("Style entry is missing a display pattern.");
        return false;
    }

    if (!style["speed"].is<JsonVariant>())
    {
        debugPrintln("Style entry is missing a speed.");
        return false;
    }

    PatternData patternData;
    String colorPattern(style["colorPattern"].as<std::string>().c_str());
    patternData.colorPattern = ColorPatternTypeHelper::fromString(colorPattern);
    String displayPattern(style["displayPattern"].as<std::string>().c_str());
    patternData.displayPattern = DisplayPatternTypeHelper::fromString(displayPattern);
    // Set colors first, since the pattern fields overlap with the last byte of the color fields.
    // Colors are considered strings in the JSON to support hex codes.
    std::string color1 = style["color1"].as<std::string>();
    std::string color2 = style["color2"].as<std::string>();
    std::string color3 = style["color3"].as<std::string>();
    std::string color4 = style["color4"].as<std::string>();
    patternData.color1 = strtoul(color1.c_str(), NULL, 0);
    patternData.color2 = strtoul(color2.c_str(), NULL, 0);
    patternData.color3 = strtoul(color3.c_str(), NULL, 0);
    patternData.color4 = strtoul(color4.c_str(), NULL, 0);
    patternData.param1 = style["param1"].as<byte>();
    patternData.param2 = style["param2"].as<byte>();
    patternData.param3 = style["param3"].as<byte>();
    patternData.param4 = style["param4"].as<byte>();
    patternData.param5 = style["param5"].as<byte>();
    patternData.param6 = style["param6"].as<byte>();

    styleDefinition.setPatternData(patternData);
    styleDefinition.setSpeed(style["speed"].as<byte>());

    return true;
}

StyleDefinition StyleConfiguration::getUnknownDefaultStyle()
{
    PatternData patternData;
    patternData.colorPattern = ColorPatternType::SingleColor;
    patternData.displayPattern = DisplayPatternType::Solid;
    patternData.color1 = Colors::Pink;

    StyleDefinition styleDefinition(patternData, 1);
    styleDefinition.setName("UnknownDefault");

    return styleDefinition;
}

void StyleConfiguration::debugPrint(const char* message)
{
#ifdef PIO_UNIT_TESTING
    printf("%s", message);
#else
    Serial.print(message);
#endif      
}

void StyleConfiguration::debugPrintln(const char* message)
{
#ifdef PIO_UNIT_TESTING
    printf("%s\n", message);
#else
    Serial.println(message);
#endif      
}