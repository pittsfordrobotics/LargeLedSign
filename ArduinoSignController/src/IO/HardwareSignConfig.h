#include <Arduino.h>

#define ORDER_SELECTOR_PINS  20, 21, 5 // The set of GPIO pin #s that tell the controller what position the sign should be in (MSB to LSB).
#define STYLE_TYPE_SELECTOR_PINS 16, 17, 18, 19  // The set of GPIO pin #s that tell the controller what style (digit # or logo) the sign should be (MSB to LSB).
#define TEST_MODE_PIN 13       // The GPIO pin # that is used to put the controller into test mode. (Low brightness, and use TestMatrix display.)

class HardwareSignConfig
{
    public:
        static byte getSignType();
        static byte getSignPosition();

    private:
        HardwareSignConfig();
};