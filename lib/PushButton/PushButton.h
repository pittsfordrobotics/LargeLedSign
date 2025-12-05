#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <Arduino.h>

#define PUSHBUTTON_DEBOUNCE_INTERVAL 50
#define PUSHBUTTON_LONGPRESS 500
#define DOUBLETAP_INTERVAL 150

enum class PushButton_ButtonPressType {
    None,
    Normal,
    Long,
    Double
};

class PushButton {
    public:
        PushButton(int pinNumber, PinMode pinMode);
        void update();
        bool wasPressed();
        PushButton_ButtonPressType lastPressType();
        PinStatus rawPinStatus();
        void clearPress();
    
    private:
        int m_pinNumber{0};
        bool m_isDown{false};
        ulong m_lastDownTime{0};
        ulong m_lastUpTime{0};
        ulong m_lastTransitionTime{0};
        bool m_inDelayInterval{false};
        bool m_wasPressed{false};
        int m_consecutiveCount{0};
        PushButton_ButtonPressType m_lastPressType{PushButton_ButtonPressType::None};
        void setPressType(ulong pressTime);
};

#endif