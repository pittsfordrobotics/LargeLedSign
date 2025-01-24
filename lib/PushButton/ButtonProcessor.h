#ifndef BUTTON_PROCESSOR_H
#define BUTTON_PROCESSOR_H

#include <Arduino.h>
#include <vector>
#include <map>
#include <algorithm>
#include "PushButton.h"

typedef void (*buttonActionProcessor)(String actionName, std::vector<String> arguments);

class ButtonProcessor
{
    struct ButtonAction
        {
            std::vector<String> buttonNames;
            String actionName;
            std::vector<String> arguments;
        };

    public:
        ButtonProcessor();
        void addButtonDefinition(String buttonName, int pinNumber, PinMode pinMode);

        //void addTapAction(String buttonName, String actionName, std::vector<String> arguments = std::vector<String>());
        //void addLongTapAction(String buttonName, String actionName, std::vector<String> arguments = std::vector<String>());

        //void addCombinationAction(std::vector<String> buttonNames, String actionName, std::vector<String> arguments = std::vector<String>());
        //void addCombinationLongTapAction(std::vector<String> buttonNames, String actionName, std::vector<String> arguments = std::vector<String>());

        // consolidated: for a single button, just use one name.
        void addTapAction(std::vector<String> buttonNames, String actionName, std::vector<String> arguments = std::vector<String>());
        void addLongTapAction(std::vector<String> buttonNames, String actionName, std::vector<String> arguments = std::vector<String>());

        void setActionProcessor(buttonActionProcessor actionProcessor) { m_actionProcessor = actionProcessor; }
        
        void update();

    private:
        buttonActionProcessor m_actionProcessor;
        std::map<String, PushButton*> m_buttonMap;
        std::vector<ButtonAction> m_tapActions;
        std::vector<ButtonAction> m_longTapActions;
};

#endif