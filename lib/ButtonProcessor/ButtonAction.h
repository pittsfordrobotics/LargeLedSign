#ifndef BUTTON_ACTION_H
#define BUTTON_ACTION_H

#include <Arduino.h>
#include <vector>

class ButtonAction
{
    public:
        ButtonAction(std::vector<String> buttonNames, String actionName, std::vector<String> arguments = std::vector<String>());

        // The Id is read-only and is internally assigned at construction time.
        int getId() { return m_id; }
        std::vector<String>& getButtonNames() { return m_buttonNames; }
        String getActionName() { return m_actionName; }
        std::vector<String>& getArguments() { return m_arguments; }

    private:
        ButtonAction() {}
        static int m_nextId;
        int m_id;
        std::vector<String> m_buttonNames;
        String m_actionName;
        std::vector<String> m_arguments;
};

#endif