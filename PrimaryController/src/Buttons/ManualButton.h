#ifndef MANUALBUTTON_H
#define MANUALBUTTON_H

#include <Arduino.h>
#include "ButtonPressType.h"

#define MANUALBUTTON_DEBOUNCE_INTERVAL 50
#define MANUALBUTTON_LONGPRESS 500

class ManualButton {
    public:
        ManualButton(int pinNumber);
        void update();
        bool wasPressed();
        ButtonPressType lastPressType();
    
    private:
        int m_pinNumber{0};
        bool m_isDown{false};
        ulong m_lastDownTime{0};
        ulong m_lastUpTime{0};
        ulong m_lastTransitionTime{0};
        bool m_wasPressed{false};
        ButtonPressType m_lastPressType{ButtonPressType::None};
        void setPressType(ulong pressTime);
};

#endif