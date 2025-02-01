#include "ButtonProcessor.h"

ButtonProcessor::ButtonProcessor()
{
}

void ButtonProcessor::addButtonDefinition(String buttonName, GenericButton* button)
{
    if (m_buttonMap.find(buttonName) != m_buttonMap.end())
    {
        // button already exists
        debugPrint("Button with name '");
        debugPrint(buttonName.c_str());
        debugPrintln("' already exists");
        return;
    }

    m_buttonMap[buttonName] = button;
}

void ButtonProcessor::addTapAction(std::vector<String> buttonNames, String actionName, std::vector<String> arguments)
{
    addAction(buttonNames, actionName, arguments, m_tapActions);
}

void ButtonProcessor::addLongTapAction(std::vector<String> buttonNames, String actionName, std::vector<String> arguments)
{
    addAction(buttonNames, actionName, arguments, m_longTapActions);
}

void ButtonProcessor::addAction(
    std::vector<String>& buttonNames, 
    String& actionName, 
    std::vector<String>& arguments, 
    std::vector<ButtonAction*>& actionList)
{
    if (buttonNames.size() == 0)
    {
        debugPrintln("No buttons specified for tap action");
        return;
    }

    for (String buttonName : buttonNames)
    {
        if (m_buttonMap.find(buttonName) == m_buttonMap.end())
        {
            // button by that name doesn't exist.  Ignore the action.
            return;
        }
    }

    ButtonAction* action = new ButtonAction(buttonNames, actionName, arguments);
    actionList.push_back(action);

    // Keep them sorted by the number of buttons in the combination
    std::sort(
        actionList.begin(),
        actionList.end(),
        [](ButtonAction* &a, ButtonAction* &b)
            { return a->getButtonNames().size() > b->getButtonNames().size(); });
}

void ButtonProcessor::update() {
    // Update all the buttons
    for (auto const& mapEntry : m_buttonMap) {
        mapEntry.second->update();
    }

    bool actionProcessed = lookForAndExecuteAction(m_longTapActions, ButtonPressType::Long);

    if (!actionProcessed)
    {
        // No long tap actions were processed, so look for normal tap actions.
        actionProcessed = lookForAndExecuteAction(m_tapActions, ButtonPressType::Normal);
    }

    // If an action was run, reset all the buttons
    if (actionProcessed)
    {
        for (auto const& mapEntry : m_buttonMap) {
            mapEntry.second->clearPress();
        }
    }
}

bool ButtonProcessor::lookForAndExecuteAction(std::vector<ButtonAction*>& actionsToProcess, ButtonPressType pressType)
{
    for (ButtonAction* buttonAction : actionsToProcess) {
        bool allPressed = true;

        for (String buttonName : buttonAction->getButtonNames()) {
            if (!m_buttonMap[buttonName]->wasPressed() || m_buttonMap[buttonName]->lastPressType() != pressType) 
            {
                allPressed = false;
            }
        }

        if (allPressed)
        {
            if (m_actionProcessor)
            {
                m_actionProcessor(buttonAction->getId(), buttonAction->getActionName(), buttonAction->getArguments());
            }
            
            return true;
        }
    }

    return false;
}

void ButtonProcessor::debugPrint(const char* message)
{
#ifdef PIO_UNIT_TESTING
    printf("%s", message);
#else
    Serial.print(message);
#endif      
}

void ButtonProcessor::debugPrintln(const char* message)
{
#ifdef PIO_UNIT_TESTING
    printf("%s\n", message);
#else
    Serial.println(message);
#endif      
}