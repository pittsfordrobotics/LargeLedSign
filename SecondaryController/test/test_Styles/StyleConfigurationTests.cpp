#include <Arduino.h>
#include <unity.h>
#include <vector>
#include "StyleConfiguration.h"

const char* noDefault();
const char* singlePinkStyle();
const char* multipleStyles();

void setUp(void) {
}

void tearDown(void) {
}

void sanityTest() {
    TEST_ASSERT_EQUAL(1, 1);
}

void emptyStringGivesNoStyles() {
    const char* jsonString = "";
    StyleConfiguration config = StyleConfiguration::ParseJson(jsonString, strlen(jsonString));
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", config.getDefaultStyleName().c_str(), "Expected empty default style name");
    TEST_ASSERT_EQUAL_MESSAGE(0, config.getStyles().size(), "Number of styles is not correct.");
}

void emptyObjectGivesNoStyles() {
    const char* jsonString = "{}";
    StyleConfiguration config = StyleConfiguration::ParseJson(jsonString, strlen(jsonString));
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", config.getDefaultStyleName().c_str(), "Expected empty default style name");
    TEST_ASSERT_EQUAL_MESSAGE(0, config.getStyles().size(), "Number of styles is not correct.");
}

void invalidJsonGivesNoStyles() {
    const char* jsonString = "NotJson";
    StyleConfiguration config = StyleConfiguration::ParseJson(jsonString, strlen(jsonString));
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", config.getDefaultStyleName().c_str(), "Expected empty default style name");
    TEST_ASSERT_EQUAL_MESSAGE(0, config.getStyles().size(), "Number of styles is not correct.");
}

void configWithNoDefaultParsesCorrectly() {
    const char* jsonString = noDefault();
    StyleConfiguration config = StyleConfiguration::ParseJson(jsonString, strlen(jsonString));
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", config.getDefaultStyleName().c_str(), "Expected empty default style name");
    TEST_ASSERT_EQUAL_MESSAGE(1, config.getStyles().size(), "Number of styles is not correct.");
    StyleDefinition style = config.getStyles().at(0);
    // Just verify the name of the style - the style contents are verified other tests.
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Pink", style.getName().c_str(), "Style name is not correct.");
}

void singlePinkStyleParsesCorrectly() {
    const char* jsonString = singlePinkStyle();
    StyleConfiguration config = StyleConfiguration::ParseJson(jsonString, strlen(jsonString));
    TEST_ASSERT_EQUAL_STRING_MESSAGE("SomeDefault", config.getDefaultStyleName().c_str(), "Default style name is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(1, config.getStyles().size(), "Number of styles is not correct.");
    StyleDefinition style = config.getStyles().at(0);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Pink", style.getName().c_str(), "Style name is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(ColorPatternType::SingleColor, style.getPatternData().colorPattern, "Color pattern is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(DisplayPatternType::Solid, style.getPatternData().displayPattern, "Display pattern is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(1, style.getSpeed(), "Speed is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(0xE616A1, style.getPatternData().color1, "Color1 is not correct.");
}

void multipleStylesParseCorrectly() {
    const char* jsonString = multipleStyles();
    StyleConfiguration config = StyleConfiguration::ParseJson(jsonString, strlen(jsonString));
    TEST_ASSERT_EQUAL_STRING_MESSAGE("SomeDefault", config.getDefaultStyleName().c_str(), "Default style name is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(3, config.getStyles().size(), "Number of styles is not correct.");
    StyleDefinition style = config.getStyles().at(0);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(sanityTest);
    RUN_TEST(emptyStringGivesNoStyles);
    RUN_TEST(emptyObjectGivesNoStyles);
    RUN_TEST(invalidJsonGivesNoStyles);
    RUN_TEST(configWithNoDefaultParsesCorrectly);
    RUN_TEST(singlePinkStyleParsesCorrectly);
    RUN_TEST(multipleStylesParseCorrectly);

    // Tests for skipping invalid entries
    
    return UNITY_END();
}

const char* singlePinkStyle()
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

const char* noDefault()
{
    return "{"
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

const char* multipleStyles()
{
    return "{"
           "    \"defaultStyleName\": \"SomeDefault\","
           "    \"predefinedStyles\": ["
           "        {"
           "            \"name\": \"RainbowLava\","
           "            \"colorPattern\": \"Rainbow\","
           "            \"displayPattern\": \"Line\","
           "            \"speed\": 200,"
           "            \"param1\": 500"
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
