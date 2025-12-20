#ifdef PIO_UNIT_TESTING
#include <ArduinoFake.h>
#else
#include <Arduino.h>
#endif

#include <unity.h>
#include <vector>
#include "Utils\DebugUtils.cpp"
#include "Configuration\StyleConfiguration.cpp"
#include "Configuration\StyleDefinition.cpp"

const char* noDefault();
const char* invalidDefaultName();
const char* singlePinkStyle();
const char* multipleStyles();
const char* invalidStyleEntries();
const char* integersForDisplayAndColorEnums();

void verifyUnknownDefaultStyle(StyleDefinition& style);

void setUp(void) {
}

void tearDown(void) {
}

void emptyStringGivesNoStyles() {
    const char* jsonString = "";
    StyleConfiguration* config = StyleConfiguration::ParseJson(jsonString, strlen(jsonString));
    verifyUnknownDefaultStyle(config->getDefaultStyle());
    TEST_ASSERT_EQUAL_MESSAGE(0, config->getStyles().size(), "Number of styles is not correct.");
}

void emptyObjectGivesNoStyles() {
    const char* jsonString = "{}";
    StyleConfiguration* config = StyleConfiguration::ParseJson(jsonString, strlen(jsonString));
    verifyUnknownDefaultStyle(config->getDefaultStyle());
    TEST_ASSERT_EQUAL_MESSAGE(0, config->getStyles().size(), "Number of styles is not correct.");
}

void invalidJsonGivesNoStyles() {
    const char* jsonString = "NotJson";
    StyleConfiguration* config = StyleConfiguration::ParseJson(jsonString, strlen(jsonString));
    verifyUnknownDefaultStyle(config->getDefaultStyle());
    TEST_ASSERT_EQUAL_MESSAGE(0, config->getStyles().size(), "Number of styles is not correct.");
}

void configWithNoDefaultParsesCorrectly() {
    const char* jsonString = noDefault();
    StyleConfiguration* config = StyleConfiguration::ParseJson(jsonString, strlen(jsonString));
    // Default style should be the first style in the list.
    StyleDefinition defaultStyle = config->getDefaultStyle();
    TEST_ASSERT_EQUAL_STRING_MESSAGE("SolidRed", defaultStyle.getName().c_str(), "Default style name is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(1, config->getStyles().size(), "Number of styles is not correct.");
    StyleDefinition style = config->getStyles().at(0);
    // Just verify the name of the style - the style contents are verified other tests.
    TEST_ASSERT_EQUAL_STRING_MESSAGE("SolidRed", style.getName().c_str(), "Style name is not correct.");
}

void invalidDefaultNameUsesExpectedDefault() {
    const char* jsonString = invalidDefaultName();
    StyleConfiguration* config = StyleConfiguration::ParseJson(jsonString, strlen(jsonString));
    StyleDefinition defaultStyle = config->getDefaultStyle();
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Pink", defaultStyle.getName().c_str(), "Default style name is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(1, config->getStyles().size(), "Number of styles is not correct.");
    StyleDefinition style = config->getStyles().at(0);
    // Just verify the name of the style - the style contents are verified other tests.
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Pink", style.getName().c_str(), "Style name is not correct.");
}

void singlePinkStyleParsesCorrectly() {
    const char* jsonString = singlePinkStyle();
    StyleConfiguration* config = StyleConfiguration::ParseJson(jsonString, strlen(jsonString));
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Pink", config->getDefaultStyle().getName().c_str(), "Default style name is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(1, config->getStyles().size(), "Number of styles is not correct.");
    StyleDefinition style = config->getStyles().at(0);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Pink", style.getName().c_str(), "Style name is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(ColorPatternType::SingleColor, style.getPatternData().colorPattern, "Color pattern is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(DisplayPatternType::Solid, style.getPatternData().displayPattern, "Display pattern is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(1, style.getSpeed(), "Speed is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(0xE616A1, style.getPatternData().color1, "Color1 is not correct.");
}

void fullListParsesCorrectly() {
    const char* jsonString = multipleStyles();
    StyleConfiguration* config = StyleConfiguration::ParseJson(jsonString, strlen(jsonString));
    TEST_ASSERT_EQUAL_STRING_MESSAGE("RedPink", config->getDefaultStyle().getName().c_str(), "Default style name is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(3, config->getStyles().size(), "Number of styles is not correct.");
    StyleDefinition style0 = config->getStyles().at(0);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("RainbowLava", style0.getName().c_str(), "Style name (0) is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(ColorPatternType::Rainbow, style0.getPatternData().colorPattern, "Color pattern (0) is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(DisplayPatternType::Line, style0.getPatternData().displayPattern, "Display pattern (0) is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(200, style0.getSpeed(), "Speed (0) is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(123, style0.getPatternData().param1, "Param1 (0) is not correct.");
    StyleDefinition style1 = config->getStyles().at(1);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Pink", style1.getName().c_str(), "Style name (1) is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(ColorPatternType::SingleColor, style1.getPatternData().colorPattern, "Color pattern (1) is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(DisplayPatternType::Solid, style1.getPatternData().displayPattern, "Display pattern (1) is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(1, style1.getSpeed(), "Speed (1) is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(0xE616A1, style1.getPatternData().color1, "Color1 (1) is not correct.");
    StyleDefinition style2 = config->getStyles().at(2);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("RedPink", style2.getName().c_str(), "Style name (2) is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(ColorPatternType::TwoColor, style2.getPatternData().colorPattern, "Color pattern (2) is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(DisplayPatternType::Right, style2.getPatternData().displayPattern, "Display pattern (2) is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(50, style2.getSpeed(), "Speed (2) is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(0xE616A1, style2.getPatternData().color1, "Color1 (2) is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(0xFF0000, style2.getPatternData().color2, "Color2 (2) is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(21, style2.getPatternData().param1, "Param1 (2) is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(22, style2.getPatternData().param2, "Param2 (2) is not correct.");
}

void invalidStyleEntriesAreSkipped() {
    const char* jsonString = invalidStyleEntries();
    StyleConfiguration* config = StyleConfiguration::ParseJson(jsonString, strlen(jsonString));
    TEST_ASSERT_EQUAL_MESSAGE(3, config->getStyles().size(), "Number of styles is not correct.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("RainbowLava", config->getStyles().at(0).getName().c_str(), "Style name (0) is not correct.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Pink", config->getStyles().at(1).getName().c_str(), "Style name (1) is not correct.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("RedPink", config->getStyles().at(2).getName().c_str(), "Style name (2) is not correct.");
}

void integersCanBeUsedForDisplayAndColorEnums() {
    const char* jsonString = integersForDisplayAndColorEnums();
    StyleConfiguration* config = StyleConfiguration::ParseJson(jsonString, strlen(jsonString));
    TEST_ASSERT_EQUAL_MESSAGE(1, config->getStyles().size(), "Number of styles is not correct.");
    StyleDefinition style = config->getStyles().at(0);
    TEST_ASSERT_EQUAL_MESSAGE(ColorPatternType::Rainbow, style.getPatternData().colorPattern, "Color pattern is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(DisplayPatternType::Random, style.getPatternData().displayPattern, "Display pattern is not correct.");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(emptyStringGivesNoStyles);
    RUN_TEST(emptyObjectGivesNoStyles);
    RUN_TEST(invalidJsonGivesNoStyles);
    RUN_TEST(configWithNoDefaultParsesCorrectly);
    RUN_TEST(invalidDefaultNameUsesExpectedDefault);
    RUN_TEST(singlePinkStyleParsesCorrectly);
    RUN_TEST(fullListParsesCorrectly);
    RUN_TEST(invalidStyleEntriesAreSkipped);
    RUN_TEST(integersCanBeUsedForDisplayAndColorEnums);
    
    return UNITY_END();
}

void verifyUnknownDefaultStyle(StyleDefinition& style) {
    TEST_ASSERT_EQUAL_STRING_MESSAGE("UnknownDefault", style.getName().c_str(), "Default style name is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(ColorPatternType::SingleColor, style.getPatternData().colorPattern, "Default color pattern is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(DisplayPatternType::Solid, style.getPatternData().displayPattern, "Default display pattern is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(Colors::Pink, style.getPatternData().color1, "Default color1 is not correct.");
}

const char* invalidDefaultName()
{
    return "{"
           "    \"defaultStyleName\": \"SomeDefault\","
           "    \"predefinedStyles\": ["
           "        {"
           "            \"name\": \"Pink\","
           "            \"colorPattern\": \"SingleColor\","
           "            \"displayPattern\": \"Solid\","
           "            \"speed\": 1,"
           "            \"color1\": \"0xE616A1\""
           "        }"
           "    ]"
           "}";
}

const char* singlePinkStyle()
{
    return "{"
           "    \"defaultStyleName\": \"Pink\","
           "    \"predefinedStyles\": ["
           "        {"
           "            \"name\": \"Pink\","
           "            \"colorPattern\": \"SingleColor\","
           "            \"displayPattern\": \"Solid\","
           "            \"speed\": 1,"
           "            \"color1\": \"0xE616A1\""
           "        }"
           "    ]"
           "}";
}

const char* noDefault()
{
    return "{"
           "    \"predefinedStyles\": ["
           "        {"
           "            \"name\": \"SolidRed\","
           "            \"colorPattern\": \"SingleColor\","
           "            \"displayPattern\": \"Solid\","
           "            \"speed\": 1,"
           "            \"color1\": \"0xFF0000\""
           "        }"
           "    ]"
           "}";
}

const char* integersForDisplayAndColorEnums()
{
    return R"json(
    {
        "predefinedStyles": [
            {
                "name": "RainbowRandom",
                "colorPattern": 3,
                "displayPattern": 6,
                "speed": 1
            }
        ]
    }
    )json";
}

const char* multipleStyles()
{
    return "{"
           "    \"defaultStyleName\": \"RedPink\","
           "    \"predefinedStyles\": ["
           "        {"
           "            \"name\": \"RainbowLava\","
           "            \"colorPattern\": \"Rainbow\","
           "            \"displayPattern\": \"Line\","
           "            \"speed\": 200,"
           "            \"param1\": 123"
           "        },"
           "        {"
           "            \"name\": \"Pink\","
           "            \"colorPattern\": \"SingleColor\","
           "            \"displayPattern\": \"Solid\","
           "            \"speed\": 1,"
           "            \"color1\": \"0xE616A1\""
           "        },"
           "        {"
           "            \"name\": \"RedPink\","
           "            \"colorPattern\": \"TwoColor\","
           "            \"displayPattern\": \"Right\","
           "            \"speed\": 50,"
           "            \"color1\": \"0xE616A1\","
           "            \"color2\": \"0xFF0000\","
           "            \"param1\": 21,"
           "            \"param2\": 22"
           "        }"
           "    ]"
           "}";
}

const char* invalidStyleEntries()
{
    return "{"
           "    \"defaultStyleName\": \"RedPink\","
           "    \"predefinedStyles\": ["
           "        {"
           "            \"name\": \"RainbowLava\","
           "            \"colorPattern\": \"Rainbow\","
           "            \"displayPattern\": \"Line\","
           "            \"speed\": 200,"
           "            \"param1\": 123"
           "        },"
           "        {"
           "            \"comment\": \"BadDueToNoName\","
           "            \"colorPattern\": \"Rainbow\","
           "            \"displayPattern\": \"Line\","
           "            \"speed\": 200,"
           "            \"param1\": 123"
           "        },"           
           "        {"
           "            \"name\": \"Pink\","
           "            \"colorPattern\": \"SingleColor\","
           "            \"displayPattern\": \"Solid\","
           "            \"speed\": 1,"
           "            \"color1\": \"0xE616A1\""
           "        },"
           "        {"
           "            \"name\": \"BadDueToNoDisplay\","
           "            \"colorPattern\": \"Rainbow\","
           "            \"speed\": 200,"
           "            \"param1\": 123"
           "        },"
           "        {}," // Completely empty entry
           "        {"
           "            \"name\": \"RedPink\","
           "            \"colorPattern\": \"TwoColor\","
           "            \"displayPattern\": \"Right\","
           "            \"speed\": 50,"
           "            \"color1\": \"0xE616A1\","
           "            \"color2\": \"0xFF0000\","
           "            \"param1\": 21,"
           "            \"param2\": 22"
           "        },"
           "        {"
           "            \"name\": \"BadDueToNoColorPattern\","
           "            \"displayPattern\": \"Solid\","
           "            \"speed\": 200,"
           "            \"param1\": 123"
           "        }"
           "    ]"
           "}";
}