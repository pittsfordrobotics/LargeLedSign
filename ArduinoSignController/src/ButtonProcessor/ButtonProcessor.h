#ifndef BUTTON_PROCESSOR_H
#define BUTTON_PROCESSOR_H

#include <Arduino.h>
#include <vector>
#include <map>
#include <algorithm>
#include "GenericButton.h"
#include "ButtonAction.h"
#include "Utils\DebugUtils.h"

// The amount of time (msec) to wait before processing an action for the button press.
// This makes it much easier to detect combinations.
#define BUTTON_PROCESSOR_ACTION_DELAY 100

// The amount of time (msec) after which to clear button presses that haven't triggered an action.
#define BUTTON_PROCESSOR_PRESS_CLEAR_DELAY 500

// The "ButtonActionProcessor" is a function pointer type for processing button actions.
// It accepts the caller button ID, action name, and a list of string arguments.
typedef void (*ButtonActionProcessor)(int callerId, String actionName, std::vector<String> arguments);

typedef unsigned long (*GetElapsedMillis)();

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

        // Used to overide the default 'millis()' function for testing purposes.
        void setElapsedMillisFunction(GetElapsedMillis elapsedMillisFunction) { m_elapsedMillisFunction = elapsedMillisFunction; }

    private:
    
        ButtonActionProcessor m_actionProcessor{nullptr};
        GetElapsedMillis m_elapsedMillisFunction{nullptr};
        std::map<String, GenericButton*> m_buttonMap;
        std::vector<ButtonAction*> m_tapActions;
        std::vector<ButtonAction*> m_longTapActions;

        unsigned long getMillis();
        
        bool lookForAndExecuteAction(std::vector<ButtonAction*>& actionsToProcess, ButtonPressType pressType);
        void addAction(
            std::vector<String>& buttonNames, 
            String& actionName, 
            std::vector<String>& arguments, 
            std::vector<ButtonAction*>& actionList);
};

#endif