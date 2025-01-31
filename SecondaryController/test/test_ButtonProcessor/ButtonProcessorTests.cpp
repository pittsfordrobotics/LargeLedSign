#include <Arduino.h>
#include <unity.h>
#include <vector>

#include "ButtonProcessor.h"
#include "MockButton.h"

int lastCallerId(-1);
String lastActionName("");
std::vector<String> lastArguments;

void resetActionParameters() {
    lastCallerId = -1;
    lastActionName = "";
    lastArguments.clear();
}

void setUp(void) {
}

void tearDown(void) {
}

void processAction(int callerId, String actionName, std::vector<String> arguments) {
    lastCallerId = callerId;
    lastActionName = actionName;
    lastArguments = arguments;
}

void noButtonsDoNothing() {
    ButtonProcessor bp;
    bp.update();
    TEST_ASSERT_TRUE(true);
}

void buttonsWithNoActions() {
    ButtonProcessor bp;
    MockButton* button1 = new MockButton();
    MockButton* button2 = new MockButton();
    bp.addButtonDefinition("Button1", button1);
    bp.addButtonDefinition("Button2", button2);
    bp.update();
    TEST_ASSERT_TRUE_MESSAGE(button1->wasUpdateCalled(), "Button 1 should have been updated.");
    TEST_ASSERT_TRUE_MESSAGE(button2->wasUpdateCalled(), "Button 2 should have been updated.");
    button1->setPressType(ButtonPressType::Normal);
    button2->setPressType(ButtonPressType::Long);
    bp.update();
    TEST_ASSERT_EQUAL_MESSAGE(ButtonPressType::None, button1->lastPressType(), "Button 1 should have been reset.");
    TEST_ASSERT_EQUAL_MESSAGE(ButtonPressType::None, button2->lastPressType(), "Button 2 should have been reset.");
}

void undefinedActionProcessorDoesNothing() {
    ButtonProcessor bp;
    MockButton* button = new MockButton();
    bp.addButtonDefinition("Button1", button);
    bp.addTapAction({"Button1"}, "Action1");
    bp.addLongTapAction({"Button1"}, "Action2");
    button->setPressType(ButtonPressType::Normal);
    bp.update();
    TEST_ASSERT_TRUE(button->wasUpdateCalled());
    TEST_ASSERT_EQUAL_MESSAGE(ButtonPressType::None, button->lastPressType(), "Button should have been reset after tap.");
    button->setPressType(ButtonPressType::Long);
    bp.update();
    TEST_ASSERT_TRUE(button->wasUpdateCalled());
    TEST_ASSERT_EQUAL_MESSAGE(ButtonPressType::None, button->lastPressType(), "Button should have been reset after long tap.");
}

void simpleActionsWithProcessor() {
    ButtonProcessor bp;
    bp.setActionProcessor(processAction);
    MockButton* button1 = new MockButton();
    MockButton* button2 = new MockButton();
    bp.addButtonDefinition("Button1", button1);
    bp.addButtonDefinition("Button2", button2);
    bp.addTapAction({"Button1"}, "Action1");
    bp.addTapAction({"Button2"}, "Action2", {"arg1", "arg2"});
    bp.addLongTapAction({"Button1"}, "Action3");
    
    button1->setPressType(ButtonPressType::Normal);
    bp.update();
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Action1", lastActionName.c_str(), "Button1's action was not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(0, lastArguments.size(), "Expected no arguments for the first action.");
    TEST_ASSERT_EQUAL_MESSAGE(ButtonPressType::None, button1->lastPressType(), "Button1 should have been reset.");

    resetActionParameters();
    button2->setPressType(ButtonPressType::Normal);
    bp.update();
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Action2", lastActionName.c_str(), "Button2's action was not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(2, lastArguments.size(), "Expected 2 arguments for the second action.");
    TEST_ASSERT_EQUAL_MESSAGE(ButtonPressType::None, button2->lastPressType(), "Button2 should have been reset.");

    resetActionParameters();
    button1->setPressType(ButtonPressType::Long);
    bp.update();
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Action3", lastActionName.c_str(), "Button1's long tap action was not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(0, lastArguments.size(), "Expected no arguments for the long tap action.");
    TEST_ASSERT_EQUAL_MESSAGE(ButtonPressType::None, button1->lastPressType(), "Button1 should have been reset after long tap.");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(noButtonsDoNothing);
    RUN_TEST(buttonsWithNoActions);
    RUN_TEST(undefinedActionProcessorDoesNothing);
    RUN_TEST(simpleActionsWithProcessor);
    
    return UNITY_END();
}