#ifndef MOCK_BUTTON_H
#define MOCK_BUTTON_H

#include "ButtonProcessor\GenericButton.h"

class MockButton : public GenericButton
{
    public:
        MockButton();
        virtual void update() { m_wasUpdateCalled = true; }
        virtual bool wasPressed() { return m_lastPressType != ButtonPressType::None; }
        virtual ButtonPressType lastPressType() { return m_lastPressType; }
        virtual void clearPress() { m_lastPressType = ButtonPressType::None; }

        void resetMock() {m_wasUpdateCalled = false; m_lastPressType = ButtonPressType::None; }
        void setPressType(ButtonPressType pressType) { m_lastPressType = pressType; }
        bool wasUpdateCalled() { return m_wasUpdateCalled; }
    
    private:
        ButtonPressType m_lastPressType{ButtonPressType::None};
        bool m_wasUpdateCalled{false};
};

#endif