#include "ButtonProcessor.h"

ButtonProcessor::ButtonProcessor()
{
}

void ButtonProcessor::addButtonDefinition(String buttonName, GenericButton* button)
{
    // if (m_buttonMap.find(buttonName) != m_buttonMap.end())
    // {
    //     // button already exists
    //     debugPrint("Button with name '");
    //     debugPrint(buttonName.c_str());
    //     debugPrintln("' already exists");
    //     return;
    // }

    m_buttonMap.insert(std::pair<String, GenericButton*>(buttonName, button));
    //m_buttonMap[buttonName] = button;
}

void ButtonProcessor::addTapAction(std::vector<String> buttonNames, String actionName, std::vector<String> arguments)
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
            // button by that name doesn't exist.  Ignore the tap action.
            return;
        }
    }

    ButtonAction* action = new ButtonAction(buttonNames, actionName, arguments);
    m_tapActions.push_back(action);

    // Keep them sorted by the number of buttons in the combination
    std::sort(
        m_tapActions.begin(),
        m_tapActions.end(),
        [](ButtonAction* &a, ButtonAction* &b)
            { return a->getButtonNames().size() > b->getButtonNames().size(); });
}

void ButtonProcessor::addLongTapAction(std::vector<String> buttonNames, String actionName, std::vector<String> arguments)
{
    if (buttonNames.size() == 0)
    {
        debugPrintln("No buttons specified for long tap action");
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

    ButtonAction* action = new ButtonAction(buttonNames, actionName, arguments);

    m_longTapActions.push_back(action);

    // Keep them sorted by the number of buttons in the combination
    std::sort(
        m_longTapActions.begin(),
        m_longTapActions.end(),
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
        lookForAndExecuteAction(m_tapActions, ButtonPressType::Normal);
    }
}

bool ButtonProcessor::lookForAndExecuteAction(std::vector<ButtonAction*> actionsToProcess, ButtonPressType pressType)
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
            
            for (String buttonName : buttonAction->getButtonNames()) {
                m_buttonMap[buttonName]->clearPress();
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