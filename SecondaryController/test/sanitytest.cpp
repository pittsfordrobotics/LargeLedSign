#include <unity.h>

void setUp(void) {

}

void tearDown(void) {

}

void sanity_test() {
    TEST_ASSERT_EQUAL(1, 1);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(sanity_test);
    
    UNITY_END();

    return 0;
}