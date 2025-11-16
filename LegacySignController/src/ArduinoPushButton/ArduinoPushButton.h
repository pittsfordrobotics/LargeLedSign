#ifndef ARDUINO_PUSHBUTTON_H
#define ARDUINO_PUSHBUTTON_H

#include <Arduino.h>
#include "../ButtonProcessor/GenericButton.h"

#define PUSHBUTTON_DEBOUNCE_INTERVAL 50
#define PUSHBUTTON_LONGPRESS 500
#define DOUBLETAP_INTERVAL 150

class ArduinoPushButton : public GenericButton {
    public:
        ArduinoPushButton(int pinNumber, PinMode pinMode);
        virtual void update();
        virtual bool wasPressed();
        virtual ButtonPressType lastPressType();
        virtual void clearPress();
        PinStatus rawPinStatus();
    
    private:
        int m_pinNumber{0};
        bool m_isDown{false};
        ulong m_lastDownTime{0};
        ulong m_lastUpTime{0};
        ulong m_lastTransitionTime{0};
        bool m_inDelayInterval{false};
        bool m_wasPressed{false};
        int m_consecutiveCount{0};
        ButtonPressType m_lastPressType{ButtonPressType::None};
        void setPressType(ulong pressTime);
};

#endif