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
    // No action was done, so the buttons shouldn't have been reset.
    TEST_ASSERT_EQUAL_MESSAGE(ButtonPressType::Normal, button1->lastPressType(), "Button 1 should have been reset.");
    TEST_ASSERT_EQUAL_MESSAGE(ButtonPressType::Long, button2->lastPressType(), "Button 2 should have been reset.");
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
    resetActionParameters();
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

void buttonsClearAfterAction() {
    resetActionParameters();
    ButtonProcessor bp;
    bp.setActionProcessor(processAction);
    MockButton* button1 = new MockButton();
    MockButton* button2 = new MockButton();
    bp.addButtonDefinition("Button1", button1);
    bp.addButtonDefinition("Button2", button2);
    bp.addTapAction({"Button1"}, "Action1");
    button1->setPressType(ButtonPressType::Normal);
    button2->setPressType(ButtonPressType::Normal);
    bp.update();
    // Both buttons should have been reset after the action.
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Action1", lastActionName.c_str(), "Button action was not correct.");
    TEST_ASSERT_EQUAL_MESSAGE(ButtonPressType::None, button1->lastPressType(), "Button1 should have been reset after long tap.");
    TEST_ASSERT_EQUAL_MESSAGE(ButtonPressType::None, button2->lastPressType(), "Button2 should have been reset after long tap.");
}

void duplicateButtonsAreIgnored() {
    resetActionParameters();
    ButtonProcessor bp;
    bp.setActionProcessor(processAction);
    MockButton* button1 = new MockButton();
    MockButton* button2 = new MockButton();
    bp.addButtonDefinition("Button", button1);
    // Button 2 should be ignored.
    bp.addButtonDefinition("Button", button2);
    bp.addTapAction({"Button"}, "Action1");
    button2->setPressType(ButtonPressType::Normal);
    bp.update();
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", lastActionName.c_str(), "No action should have been run.");
}

void combinationActionTests() {
    resetActionParameters();
    ButtonProcessor bp;
    bp.setActionProcessor(processAction);
    MockButton* button1 = new MockButton();
    MockButton* button2 = new MockButton();
    MockButton* button3 = new MockButton();
    bp.addButtonDefinition("Button1", button1);
    bp.addButtonDefinition("Button2", button2);
    bp.addButtonDefinition("Button3", button3);
    bp.addTapAction({"Button1", "Button2"}, "Action1-2");
    bp.addTapAction({"Button2"}, "Action2"); // This action shouldn't be called
    bp.addTapAction({"Button1", "Button2", "Button3"}, "Action1-2-3");
    button1->setPressType(ButtonPressType::Normal);
    bp.update();
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", lastActionName.c_str(), "No action should have been run.");
    button2->setPressType(ButtonPressType::Normal);
    button3->setPressType(ButtonPressType::Normal);
    bp.update();
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Action1-2-3", lastActionName.c_str(), "Button1/2/3 action was not correct.");
    button1->setPressType(ButtonPressType::Normal);
    button2->setPressType(ButtonPressType::Normal);
    bp.update();
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Action1-2", lastActionName.c_str(), "Button1/2 action was not correct.");
}

void unknownButtonNamesInActionsAreIgnored() {
    resetActionParameters();
    ButtonProcessor bp;
    bp.setActionProcessor(processAction);
    MockButton* button1 = new MockButton();
    MockButton* button2 = new MockButton();
    bp.addButtonDefinition("Button1", button1);
    bp.addButtonDefinition("Button2", button2);
    bp.addTapAction({"ButtonX", "Button1"}, "Action1");
    bp.addTapAction({"ButtonY"}, "Action2");
    button1->setPressType(ButtonPressType::Normal);
    bp.update();
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", lastActionName.c_str(), "No action should have been done.");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(noButtonsDoNothing);
    RUN_TEST(buttonsWithNoActions);
    RUN_TEST(undefinedActionProcessorDoesNothing);
    RUN_TEST(simpleActionsWithProcessor);
    RUN_TEST(buttonsClearAfterAction);
    RUN_TEST(duplicateButtonsAreIgnored);
    RUN_TEST(combinationActionTests);
    RUN_TEST(unknownButtonNamesInActionsAreIgnored);
    
    return UNITY_END();
}