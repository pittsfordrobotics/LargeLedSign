#include <unity.h>

void setUp(void) {
}

void tearDown(void) {
}

void sanityTest(void) {
    TEST_ASSERT_EQUAL(1, 1);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(sanityTest);
    
    return UNITY_END();
}