#include "HardwareSignConfig.h"

byte HardwareSignConfig::getSignType()
{
    pinMode(TEST_MODE_PIN, INPUT_PULLUP);
    std::vector<int> typeSelectorPins = {STYLE_TYPE_SELECTOR_PINS};

    for (uint i = 0; i < typeSelectorPins.size(); i++)
    {
        pinMode(typeSelectorPins.at(i), INPUT_PULLUP);
    }

    delay(100);

    // Check the "test" pin
    if (digitalRead(TEST_MODE_PIN) == LOW)
    {
        Serial.println("Test mode detected. Returning 255 for TestMatrix sign type.");
        return 255;
    }

    // Pull the pin low to indicate active.
    byte type = 0;
    for (uint i = 0; i < typeSelectorPins.size(); i++)
    {
        type = type << 1;
        if (digitalRead(typeSelectorPins.at(i)) == LOW)
        {
            type++;
        }
    }

    return type;
}

byte HardwareSignConfig::getSignPosition()
{
    std::vector<int> orderSelectorPins = {ORDER_SELECTOR_PINS};

    for (uint i = 0; i < orderSelectorPins.size(); i++)
    {
        pinMode(orderSelectorPins.at(i), INPUT_PULLUP);
    }

    delay(100);

    // Pull the pin low to indicate active.
    byte order = 0;
    for (uint i = 0; i < orderSelectorPins.size(); i++)
    {
        order = order << 1;
        if (digitalRead(orderSelectorPins.at(i)) == LOW)
        {
            order++;
        }
    }

    return order;
}
