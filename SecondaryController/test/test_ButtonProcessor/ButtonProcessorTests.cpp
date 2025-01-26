#include <Arduino.h>
#include <unity.h>
#include <vector>

#include "ButtonProcessor.h"
#include "MockButton.h"

void setUp(void) {
}

void tearDown(void) {
}

void sanityTest() {
    ButtonProcessor bp;
    bp.addButtonDefinition("Button", new MockButton());
    TEST_ASSERT_TRUE(true);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(sanityTest);
    
    return UNITY_END();
}