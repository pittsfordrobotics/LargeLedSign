#ifndef MANUALBUTTON_H
#define MANUALBUTTON_H

#include <Arduino.h>

#define MANUALBUTTON_DEBOUNCE_INTERVAL 50
#define MANUALBUTTON_LONGPRESS 500
#define DOUBLETAP_INTERVAL 150

enum class ButtonPressType {
    None,
    Normal,
    Long,
    Double
};

class ManualButton {
    public:
        ManualButton(int pinNumber, PinMode pinMode);
        void update();
        bool wasPressed();
        ButtonPressType lastPressType();
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
        ButtonPressType m_lastPressType{ButtonPressType::None};
        void setPressType(ulong pressTime);
};

#endif