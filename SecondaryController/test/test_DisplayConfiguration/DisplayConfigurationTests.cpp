typedef	unsigned int	uint;		/* System V compatibility */
typedef	unsigned long	ulong;		/* System V compatibility */

#include <stdint.h>
#include <Arduino.h>
#include <unity.h>
#include <vector>
#include "DisplayConfiguration.h"


void setUp(void) {

}

void tearDown(void) {

}

void parseJsonWithEmptyString() {
    std::vector<DisplayConfiguration*>* configs = DisplayConfiguration::ParseJson("");
    TEST_ASSERT_EQUAL(0, configs->size());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(parseJsonWithEmptyString);
    
    return UNITY_END();
}