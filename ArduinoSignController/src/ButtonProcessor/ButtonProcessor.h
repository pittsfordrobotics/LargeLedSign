#ifndef BUTTON_PROCESSOR_H
#define BUTTON_PROCESSOR_H

#include <Arduino.h>
#include <vector>
#include <map>
#include <algorithm>
#include "GenericButton.h"
#include "ButtonAction.h"
#include "Utils\DebugUtils.h"

// The "ButtonActionProcessor" is a function pointer type for processing button actions.
// It accepts the caller button ID, action name, and a list of string arguments.
typedef void (*ButtonActionProcessor)(int callerId, String actionName, std::vector<String> arguments);

class ButtonProcessor
{
    public:
        ButtonProcessor();
        void addButtonDefinition(String buttonName, GenericButton* button);

        void addTapAction(std::vector<String> buttonNames, String actionName, std::vector<String> arguments = std::vector<String>());
        void addLongTapAction(std::vector<String> buttonNames, String actionName, std::vector<String> arguments = std::vector<String>());

        void setActionProcessor(ButtonActionProcessor actionProcessor) { m_actionProcessor = actionProcessor; }
        
        void update();

        std::vector<GenericButton*> getButtons();

    private:
        ButtonActionProcessor m_actionProcessor{nullptr};
        std::map<String, GenericButton*> m_buttonMap;
        std::vector<ButtonAction*> m_tapActions;
        std::vector<ButtonAction*> m_longTapActions;

        bool lookForAndExecuteAction(std::vector<ButtonAction*>& actionsToProcess, ButtonPressType pressType);
        void addAction(
            std::vector<String>& buttonNames, 
            String& actionName, 
            std::vector<String>& arguments, 
            std::vector<ButtonAction*>& actionList);
};

#endif