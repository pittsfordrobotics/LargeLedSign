#ifndef SYSTEMCONFIGURATION_H
#define SYSTEMCONFIGURATION_H

#include <Arduino.h>
#include <functional>
#include <vector>
#include <ArduinoJson.h>
#include "..\ButtonProcessor\ButtonProcessor.h"
#include "..\DebugUtils\DebugUtils.h"

typedef GenericButton* (*ButtonFactory)(int gpioPin);

class SystemConfiguration {
    public:
        static SystemConfiguration* ParseJson(
            const char* jsonString, 
            size_t length,
            ButtonActionProcessor actionProcessor,
            ButtonFactory buttonFactory);

        ButtonProcessor& getButtonProcessor() { return m_buttonProcessor; }

    private:
        ButtonProcessor m_buttonProcessor;

        SystemConfiguration() {};
        void configureButtonProcessor(JsonVariant buttonConfigs, ButtonFactory buttonFactory);
        void addButtonDefinitions(JsonArray definitions, ButtonFactory buttonFactory);
        void addButtonActions(JsonArray actions);
        std::vector<String> getStringList(JsonVariant array);
};

#endif