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
    m_lastPressType = ButtonPressType::None;
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
        // Button is down.
        if (!m_isDown)
        {
            // Button was up, but is down now.
            // Check to see if we're outside the debounce window
            if (millis() > m_lastTransitionTime + MANUALBUTTON_DEBOUNCE_INTERVAL)
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
            if (millis() > m_lastTransitionTime + MANUALBUTTON_DEBOUNCE_INTERVAL)
            {
                m_lastUpTime = millis();
                m_lastTransitionTime = m_lastUpTime;
                m_isDown = false;
                m_inDelayInterval = true;
            }
        }
        else
        {
            // If we had been pressed but and are in the delay interval,
            // see if enough time has passed.
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

void ManualButton::setPressType(ulong pressTime)
{
    if (pressTime > MANUALBUTTON_LONGPRESS)
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