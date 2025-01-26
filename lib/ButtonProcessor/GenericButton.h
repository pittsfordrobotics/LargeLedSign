#ifndef GENERIC_BUTTON_H
#define GENERIC_BUTTON_H

enum class ButtonPressType {
    None,
    Normal,
    Long,
    Double
};

class GenericButton {
    public:
        virtual void update() = 0;
        virtual bool wasPressed() = 0;
        virtual ButtonPressType lastPressType() = 0;
        virtual void clearPress() = 0;;
};

#endif