#include "ManualButton.h"

ManualButton::ManualButton(int pinNumber, PinMode mode)
{
    pinMode(pinNumber, mode);
    m_pinNumber = pinNumber;
}

bool ManualButton::wasPressed()
{
    bool pressed = m_wasPressed;
    return pressed;
}

void ManualButton::clearPress()
{
    m_wasPressed = false;
}

ButtonPressType ManualButton::lastPressType()
{
    return m_lastPressType;
}

PinStatus ManualButton::rawPinStatus()
{
    return digitalRead(m_pinNumber);
}

void ManualButton::update()
{
    if (digitalRead(m_pinNumber) == LOW)
    {
        // Button is currently pressed.
        if (!m_isDown)
        {
            // Button was up, but is down now.
            // Check to see if we're in the debounce window
            if (millis() > m_lastTransitionTime + MANUALBUTTON_DEBOUNCE_INTERVAL)
            {
                m_lastDownTime = millis();
                m_lastTransitionTime = m_lastDownTime;
                m_isDown = true;
            }
        }
    }
    else
    {
        // Button is not pressed.
        if (m_isDown)
        {
            // Button was down, but is up now.
            // check to see if we're in the debounce window
            if (millis() > m_lastTransitionTime + MANUALBUTTON_DEBOUNCE_INTERVAL)
            {
                m_lastUpTime = millis();
                m_lastTransitionTime = m_lastUpTime;
                m_isDown = false;
                m_wasPressed = true;

                ulong pressTime = m_lastUpTime - m_lastDownTime;
                setPressType(pressTime);
            }
        }
    }
}

void ManualButton::setPressType(ulong pressTime)
{
    if (pressTime > MANUALBUTTON_LONGPRESS)
    {
        m_lastPressType = ButtonPressType::Long;
    }
    else
    {
        m_lastPressType = ButtonPressType::Normal;
    }
}