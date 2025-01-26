#include "ButtonProcessor.h"

ButtonProcessor::ButtonProcessor()
{
}

void ButtonProcessor::addButtonDefinition(String buttonName, GenericButton* button)
{
    if (m_buttonMap.find(buttonName) != m_buttonMap.end())
    {
        // button already exists
        Serial.println("Button with name '" + buttonName + "' already exists");
        return;
    }

    m_buttonMap[buttonName] = button;
}

void ButtonProcessor::addTapAction(std::vector<String> buttonNames, String actionName, std::vector<String> arguments)
{
    if (buttonNames.size() == 0)
    {
        Serial.println("No buttons specified for tap action");
        return;
    }

    for (String buttonName : buttonNames)
    {
        if (m_buttonMap.find(buttonName) == m_buttonMap.end())
        {
            // button by that name doesn't exist.  Ignore the tap action.
            return;
        }
    }

    ButtonAction action;
    action.buttonNames = buttonNames;
    action.actionName = actionName;
    action.arguments = arguments;

    m_tapActions.push_back(action);

    // Keep them sorted by the number of buttons in the combination
    std::sort(
        m_tapActions.begin(),
        m_tapActions.end(),
        [](ButtonAction &a, ButtonAction &b)
            { return a.buttonNames.size() > b.buttonNames.size(); });
}

void ButtonProcessor::addLongTapAction(std::vector<String> buttonNames, String actionName, std::vector<String> arguments)
{
    if (buttonNames.size() == 0)
    {
        Serial.println("No buttons specified for long tap action");
        return;
    }

    for (String buttonName : buttonNames)
    {
        if (m_buttonMap.find(buttonName) == m_buttonMap.end())
        {
            // button by that name doesn't exist.  Ignore the tap action.
            return;
        }
    }

    ButtonAction action;
    action.buttonNames = buttonNames;
    action.actionName = actionName;
    action.arguments = arguments;

    m_longTapActions.push_back(action);

    // Keep them sorted by the number of buttons in the combination
    std::sort(
        m_longTapActions.begin(),
        m_longTapActions.end(),
        [](ButtonAction &a, ButtonAction &b)
            { return a.buttonNames.size() > b.buttonNames.size(); });
}

void ButtonProcessor::update() {
    // Update all the buttons
    for (auto const& mapEntry : m_buttonMap) {
        mapEntry.second->update();
    }

    // Look for long tap actions first, then tap actions
    for (ButtonAction longTapAction : m_longTapActions) {
        bool allPressed = true;

        for (String buttonName : longTapAction.buttonNames) {
            if (!m_buttonMap[buttonName]->wasPressed() || m_buttonMap[buttonName]->lastPressType() != ButtonPressType::Long) 
            {
                allPressed = false;
            }
        }

        if (allPressed)
        {
            m_actionProcessor(longTapAction.actionName, longTapAction.arguments);
            
            for (String buttonName : longTapAction.buttonNames) {
                m_buttonMap[buttonName]->clearPress();
            }
            
            return;
        }
    }

    // No long tap actions found, so check for tap actions
    for (ButtonAction tapAction : m_tapActions)
    {
        bool allPressed = true;

        for (String buttonName : tapAction.buttonNames)
        {
            if (!m_buttonMap[buttonName]->wasPressed() || m_buttonMap[buttonName]->lastPressType() != ButtonPressType::Normal)
            {
                allPressed = false;
            }
        }

        if (allPressed)
        {
            m_actionProcessor(tapAction.actionName, tapAction.arguments);

            for (String buttonName : tapAction.buttonNames)
            {
                m_buttonMap[buttonName]->clearPress();
            }

            return;
        }
    }
}