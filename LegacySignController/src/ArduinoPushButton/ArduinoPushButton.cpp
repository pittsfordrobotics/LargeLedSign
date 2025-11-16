#include "ArduinoPushButton.h"

ArduinoPushButton::ArduinoPushButton(int pinNumber, PinMode mode)
{
    pinMode(pinNumber, mode);
    m_pinNumber = pinNumber;
}

bool ArduinoPushButton::wasPressed()
{
    bool pressed = m_wasPressed;
    return pressed;
}

void ArduinoPushButton::clearPress()
{
    m_wasPressed = false;
    m_lastPressType = ButtonPressType::None;
}

ButtonPressType ArduinoPushButton::lastPressType()
{
    return m_lastPressType;
}

PinStatus ArduinoPushButton::rawPinStatus()
{
    return digitalRead(m_pinNumber);
}

void ArduinoPushButton::update()
{
    if (digitalRead(m_pinNumber) == LOW)
    {
        // Button is down.
        if (!m_isDown)
        {
            // Button was up, but is down now.
            // Check to see if we're outside the debounce window
            if (millis() > m_lastTransitionTime + PUSHBUTTON_DEBOUNCE_INTERVAL)
            {
                m_lastDownTime = millis();
                m_lastTransitionTime = m_lastDownTime;
                m_isDown = true;
            }
            if (millis() > m_lastUpTime + DOUBLETAP_INTERVAL)
            {
                // Outside the double-tap window, so treat this as a new tap series.
                m_consecutiveCount = 1;
            }
            else
            {
                // Inside the double-tap window
                m_consecutiveCount++;
            }
        }
    }
    else
    {
        // Button is up.
        if (m_isDown)
        {
            // Button was down, but is up now.
            // check to see if we're outside the debounce window
            if (millis() > m_lastTransitionTime + PUSHBUTTON_DEBOUNCE_INTERVAL)
            {
                m_lastUpTime = millis();
                m_lastTransitionTime = m_lastUpTime;
                m_isDown = false;
                m_inDelayInterval = true;
            }
        }
        else
        {
            // If we're inside the delay interval, the next press would be treated
            // as a double-press.  If we've exited the delay interval, count this
            // as an actual press and stop waiting.
            if (m_inDelayInterval && (millis() > m_lastUpTime + DOUBLETAP_INTERVAL))
            {
                m_inDelayInterval = false;
                m_wasPressed = true;
                ulong pressTime = m_lastUpTime - m_lastDownTime;
                setPressType(pressTime);
            }
        }
    }
}

void ArduinoPushButton::setPressType(ulong pressTime)
{
    if (pressTime > PUSHBUTTON_LONGPRESS)
    {
        m_lastPressType = ButtonPressType::Long;
    }
    else
    {
        if (m_consecutiveCount > 1)
        {
            m_lastPressType = ButtonPressType::Double;
        }
        else
        {
            m_lastPressType = ButtonPressType::Normal;
        }
    }
}