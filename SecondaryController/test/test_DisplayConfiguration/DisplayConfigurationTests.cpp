#include <Arduino.h>
#include <unity.h>
#include <vector>
#include "DisplayConfiguration.h"

void setUp(void) {

}

void tearDown(void) {

}

void parseJsonWithEmptyString() {
    const char* jsonString = "";
    std::vector<DisplayConfiguration*>* configs = DisplayConfiguration::ParseJson(jsonString);
    TEST_ASSERT_EQUAL(0, configs->size());
}

void parseJsonWithEmptyObject() {
    const char* jsonString = "{}";
    std::vector<DisplayConfiguration*>* configs = DisplayConfiguration::ParseJson(jsonString);
    TEST_ASSERT_EQUAL(0, configs->size());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(parseJsonWithEmptyString);
    RUN_TEST(parseJsonWithEmptyObject);
    
    return UNITY_END();
}