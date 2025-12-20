#include "ButtonAction.h"

int ButtonAction::m_nextId = 0;

ButtonAction::ButtonAction(std::vector<String> buttonNames, String actionName, std::vector<String> arguments)
{
    m_actionName = actionName;
    m_id = m_nextId++;

    for (String buttonName : buttonNames)
    {
        m_buttonNames.push_back(buttonName);
    }

    for (String argument : arguments)
    {
        m_arguments.push_back(argument);
    }
}
