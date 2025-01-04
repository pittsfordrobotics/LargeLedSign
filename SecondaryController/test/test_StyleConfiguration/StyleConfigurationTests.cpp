#include <Arduino.h>
#include <unity.h>
#include <vector>
#include "StyleDefinition.h"

void setUp(void) {
}

void tearDown(void) {
}

void sanityTest() {
    TEST_ASSERT_EQUAL(1, 1);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(sanityTest);
    
    return UNITY_END();
}
