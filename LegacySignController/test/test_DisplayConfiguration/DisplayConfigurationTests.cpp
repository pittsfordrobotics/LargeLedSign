#ifdef PIO_UNIT_TESTING
#include <ArduinoFake.h>
#else
#include <Arduino.h>
#endif

#include <unity.h>
#include <vector>
#include "Utils\DebugUtils.cpp"
#include "Configuration\DisplayConfiguration.cpp"

const char* fullTestMatrixJson();
const char* minimalTestMatrixJson();
const char* disabledDisplayJson();
const char* defaultBrightnessTestJson();

void setUp(void) {
}

void tearDown(void) {
}

void emptyStringGivesNoConfigs() {
    const char* jsonString = "";
    std::vector<DisplayConfiguration>* configs = DisplayConfiguration::ParseJson(jsonString, strlen(jsonString));
    TEST_ASSERT_EQUAL_MESSAGE(0, configs->size(), "Expected 0 display configurations");
    delete configs;
}

void emptyObjectGivesNoConfigs() {
    const char* jsonString = "{}";
    std::vector<DisplayConfiguration>* configs = DisplayConfiguration::ParseJson(jsonString, strlen(jsonString));
    TEST_ASSERT_EQUAL_MESSAGE(0, configs->size(), "Expected 0 display configurations");
    delete configs;
}

void invalidJsonGivesNoConfigs() {
    const char* jsonString = "NotJson";
    std::vector<DisplayConfiguration>* configs = DisplayConfiguration::ParseJson(jsonString, strlen(jsonString));
    TEST_ASSERT_EQUAL_MESSAGE(0, configs->size(), "Expected 0 display configurations");
    delete configs;
}

void emptyDisplayArrayGivesNoConfigs() {
    const char* jsonString = "{\"displays\":[]}";
    std::vector<DisplayConfiguration>* configs = DisplayConfiguration::ParseJson(jsonString, strlen(jsonString));
    TEST_ASSERT_EQUAL_MESSAGE(0, configs->size(), "Expected 0 display configurations");
    delete configs;
}

void parseFullTestMatrixJson() {
    const char* jsonString = fullTestMatrixJson();
    std::vector<DisplayConfiguration>* configs = DisplayConfiguration::ParseJson(jsonString, strlen(jsonString));
    TEST_ASSERT_EQUAL_MESSAGE(1, configs->size(), "Expected 1 display configuration");
    DisplayConfiguration config = configs->at(0);
    TEST_ASSERT_EQUAL_MESSAGE(16, config.getGpioPin(), "GPIO pin is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(64, config.getNumberOfPixels(), "Number of pixels is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(15, config.getDefaultBrightness(), "Default brightness is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(1, config.getRowsAbove(), "Rows above is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(2, config.getRowsBelow(), "Rows below is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(3, config.getColumnsToLeft(), "Columns to left is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(4, config.getColumnsToRight(), "Columns to right is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(5, config.getDigitsToLeft(), "Digits to left is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(6, config.getDigitsToRight(), "Digits to right is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(8, config.getColumnPixelMapping().size(), "Column pixel mapping size is not correct.");
    int expectedFirstColumnNumbers[] = {56,48,40,32,24,16,8,0};
    for (int i = 0; i < config.getColumnPixelMapping().size(); i++) {
        std::vector<uint16_t>* column = config.getColumnPixelMapping().at(i);
        std::string colNum = std::to_string(i);
        TEST_ASSERT_EQUAL_MESSAGE(8, column->size(), ("Column pixel mapping size is not correct. Column: " + colNum).c_str());
        TEST_ASSERT_EQUAL_MESSAGE(expectedFirstColumnNumbers[i], column->at(0), ("First pixel in column is not correct. Column: " + colNum).c_str());
    }

    TEST_ASSERT_EQUAL_MESSAGE(8, config.getRowPixelMapping().size(), "Row pixel mapping size is not correct.");
    int expectedFirstRowNumbers[] = {56,57,58,59,60,61,62,63};
    for (int i = 0; i < config.getRowPixelMapping().size(); i++) {
        std::vector<uint16_t>* row = config.getRowPixelMapping().at(i);
        std::string rowNum = std::to_string(i);
        TEST_ASSERT_EQUAL_MESSAGE(8, row->size(), ("Row pixel mapping size is not correct. Row: " + rowNum).c_str());
        TEST_ASSERT_EQUAL_MESSAGE(expectedFirstRowNumbers[i], row->at(0), ("First pixel in row is not correct. Row: " + rowNum).c_str());
    }

    TEST_ASSERT_EQUAL_MESSAGE(1, config.getDigitPixelMapping().size(), "Digit pixel mapping size is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(64, config.getDigitPixelMapping().at(0)->size(), "Digit pixel mapping size is not correct.");
    delete configs;
}

void parseMinimalTestMatrixJson() {
    const char* jsonString = minimalTestMatrixJson();
    std::vector<DisplayConfiguration>* configs = DisplayConfiguration::ParseJson(jsonString, strlen(jsonString));
    TEST_ASSERT_EQUAL_MESSAGE(1, configs->size(), "Expected 1 display configuration");
    DisplayConfiguration config = configs->at(0);
    TEST_ASSERT_EQUAL_MESSAGE(16, config.getGpioPin(), "GPIO pin is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(64, config.getNumberOfPixels(), "Number of pixels is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(100, config.getDefaultBrightness(), "Default brightness is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(0, config.getRowsAbove(), "Rows above is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(0, config.getRowsBelow(), "Rows below is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(0, config.getColumnsToLeft(), "Columns to left is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(0, config.getColumnsToRight(), "Columns to right is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(0, config.getDigitsToLeft(), "Digits to left is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(0, config.getDigitsToRight(), "Digits to right is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(8, config.getColumnPixelMapping().size(), "Column pixel mapping size is not correct.");
    int expectedFirstColumnNumbers[] = {56,48,40,32,24,16,8,0};
    for (int i = 0; i < config.getColumnPixelMapping().size(); i++) {
        std::vector<uint16_t>* column = config.getColumnPixelMapping().at(i);
        std::string colNum = std::to_string(i);
        TEST_ASSERT_EQUAL_MESSAGE(8, column->size(), ("Column pixel mapping size is not correct. Column: " + colNum).c_str());
        TEST_ASSERT_EQUAL_MESSAGE(expectedFirstColumnNumbers[i], column->at(0), ("First pixel in column is not correct. Column: " + colNum).c_str());
    }

    TEST_ASSERT_EQUAL_MESSAGE(8, config.getRowPixelMapping().size(), "Row pixel mapping size is not correct.");
    int expectedFirstRowNumbers[] = {56,57,58,59,60,61,62,63};
    for (int i = 0; i < config.getRowPixelMapping().size(); i++) {
        std::vector<uint16_t>* row = config.getRowPixelMapping().at(i);
        std::string rowNum = std::to_string(i);
        TEST_ASSERT_EQUAL_MESSAGE(8, row->size(), ("Row pixel mapping size is not correct. Row: " + rowNum).c_str());
        TEST_ASSERT_EQUAL_MESSAGE(expectedFirstRowNumbers[i], row->at(0), ("First pixel in row is not correct. Row: " + rowNum).c_str());
    }

    TEST_ASSERT_EQUAL_MESSAGE(1, config.getDigitPixelMapping().size(), "Digit pixel mapping size is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(64, config.getDigitPixelMapping().at(0)->size(), "Digit pixel mapping size is not correct.");
    delete configs;
}

void disabledDisplaysAreSkipped() {
    const char* jsonString = disabledDisplayJson();
    std::vector<DisplayConfiguration>* configs = DisplayConfiguration::ParseJson(jsonString, strlen(jsonString));
    TEST_ASSERT_EQUAL_MESSAGE(2, configs->size(), "Expected 2 display configurations");
    TEST_ASSERT_EQUAL_MESSAGE(16, configs->at(0).getGpioPin(), "First display GPIO pin is not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(18, configs->at(1).getGpioPin(), "Second display GPIO pin is not correct.");
    delete configs;
}

void defaultBrightnessIsSet() {
    const char* jsonString = defaultBrightnessTestJson();
    std::vector<DisplayConfiguration>* configs = DisplayConfiguration::ParseJson(jsonString, strlen(jsonString));
    TEST_ASSERT_EQUAL_MESSAGE(2, configs->size(), "Expected 2 display configuration");
    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_CONFIG_DEFAULTBRIGHTNESS, configs->at(0).getDefaultBrightness(), "Default brightness for the first display should be the global default value.");
    TEST_ASSERT_EQUAL_MESSAGE(99, configs->at(1).getDefaultBrightness(), "Default brightness for the second display should have been read from the config.");
    delete configs;
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(emptyStringGivesNoConfigs);
    RUN_TEST(emptyObjectGivesNoConfigs);
    RUN_TEST(emptyDisplayArrayGivesNoConfigs);
    RUN_TEST(invalidJsonGivesNoConfigs);
    RUN_TEST(parseFullTestMatrixJson);
    RUN_TEST(parseMinimalTestMatrixJson);
    RUN_TEST(disabledDisplaysAreSkipped);
    RUN_TEST(defaultBrightnessIsSet);
    
    return UNITY_END();
}

const char* fullTestMatrixJson()
{
    return  "{"
            "    \"defaultBrightness\": 100,"
            "    \"displays\": ["
            "        {"
            "            \"name\": \"TestMatrix1\","
            "            \"disabled\": false,"
            "            \"gpioPin\": 16,"
            "            \"numberOfPixels\": 64,"
            "            \"defaultBrightness\": 15,"
            "            \"rowsAbove\": 1,"
            "            \"rowsBelow\": 2,"
            "            \"columnsToLeft\": 3,"
            "            \"columnsToRight\": 4,"
            "            \"digitsToLeft\": 5,"
            "            \"digitsToRight\": 6,"
            "            \"columnPixelMapping\": ["
            "                [56,57,58,59,60,61,62,63],"
            "                [48,49,50,51,52,53,54,55],"
            "                [40,41,42,43,44,45,46,47],"
            "                [32,33,34,35,36,37,38,39],"
            "                [24,25,26,27,28,29,30,31],"
            "                [16,17,18,19,20,21,22,23],"
            "                [8,9,10,11,12,13,14,15],"
            "                [0,1,2,3,4,5,6,7]"
            "            ],"
            "            \"rowPixelMapping\": ["
            "                [56,55,40,39,24,23,8,7],"
            "                [57,54,41,38,25,22,9,6],"
            "                [58,53,42,37,26,21,10,5],"
            "                [59,52,43,36,27,20,11,4],"
            "                [60,51,44,35,28,19,12,3],"
            "                [61,50,45,34,29,18,13,2],"
            "                [62,49,46,33,30,17,14,1],"
            "                [63,48,47,32,31,16,15,0]"
            "            ],"
            "            \"digitPixelMapping\":"
            "            ["
            "                [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63]"
            "            ]"
            "        }"
            "    ]"
            "}";
}

const char* minimalTestMatrixJson()
{
    return  "{"
            "    \"defaultBrightness\": 100,"
            "    \"displays\": ["
            "        {"
            "            \"name\": \"TestMatrix1\","
            "            \"gpioPin\": 16,"
            "            \"numberOfPixels\": 64,"
            "            \"columnPixelMapping\": ["
            "                [56,57,58,59,60,61,62,63],"
            "                [48,49,50,51,52,53,54,55],"
            "                [40,41,42,43,44,45,46,47],"
            "                [32,33,34,35,36,37,38,39],"
            "                [24,25,26,27,28,29,30,31],"
            "                [16,17,18,19,20,21,22,23],"
            "                [8,9,10,11,12,13,14,15],"
            "                [0,1,2,3,4,5,6,7]"
            "            ],"
            "            \"rowPixelMapping\": ["
            "                [56,55,40,39,24,23,8,7],"
            "                [57,54,41,38,25,22,9,6],"
            "                [58,53,42,37,26,21,10,5],"
            "                [59,52,43,36,27,20,11,4],"
            "                [60,51,44,35,28,19,12,3],"
            "                [61,50,45,34,29,18,13,2],"
            "                [62,49,46,33,30,17,14,1],"
            "                [63,48,47,32,31,16,15,0]"
            "            ]"
            "        }"
            "    ]"
            "}";
}

const char* disabledDisplayJson()
{
    return  "{"
            "    \"displays\": ["
            "        {"
            "            \"name\": \"Dummy1\","
            "            \"gpioPin\": 16,"
            "            \"numberOfPixels\": 4,"
            "            \"columnPixelMapping\": [[0,1,2,3]],"
            "            \"rowPixelMapping\": [[0,1,2,3]]"
            "        },"
            "        {"
            "            \"name\": \"Dummy2\","
            "            \"disabled\": true,"
            "            \"gpioPin\": 17,"
            "            \"numberOfPixels\": 4,"
            "            \"columnPixelMapping\": [[0,1,2,3]],"
            "            \"rowPixelMapping\": [[0,1,2,3]]"
            "        },"
            "        {"
            "            \"name\": \"Dummy3\","
            "            \"disabled\": false,"
            "            \"gpioPin\": 18,"
            "            \"numberOfPixels\": 4,"
            "            \"columnPixelMapping\": [[0,1,2,3]],"
            "            \"rowPixelMapping\": [[0,1,2,3]]"
            "        }"
            "    ]"
            "}";
}

const char* defaultBrightnessTestJson()
{
    return  "{"
            "    \"displays\": ["
            "        {"
            "            \"name\": \"Dummy1\","
            "            \"gpioPin\": 16,"
            "            \"numberOfPixels\": 4,"
            "            \"columnPixelMapping\": [[0,1,2,3]],"
            "            \"rowPixelMapping\": [[0,1,2,3]]"
            "        },"
            "        {"
            "            \"name\": \"Dummy2\","
            "            \"defaultBrightness\": 99,"
            "            \"gpioPin\": 17,"
            "            \"numberOfPixels\": 4,"
            "            \"columnPixelMapping\": [[0,1,2,3]],"
            "            \"rowPixelMapping\": [[0,1,2,3]]"
            "        }"
            "    ]"
            "}";
}