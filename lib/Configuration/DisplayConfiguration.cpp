#include "DisplayConfiguration.h"

std::vector<DisplayConfiguration>* DisplayConfiguration::ParseJson(const char* jsonString, size_t length)
{
    std::vector<DisplayConfiguration>* configs = new std::vector<DisplayConfiguration>();

    JsonDocument configDoc;
    DeserializationError err = deserializeJson(configDoc, jsonString, length);
    
    if (err == DeserializationError::EmptyInput)
    {
        debugPrintln("Display configuration JSON string was empty.");
        return configs;
    }

    if (err != DeserializationError::Ok)
    {
        debugPrint("Error parsing display configuration JSON: ");
        debugPrintln(err.c_str());
        return configs;
    }

    if (configDoc.isNull())
    {
        debugPrintln("Display configuration JSON was null after parsing.");
        return configs;
    }

    byte defaultBrightness = DISPLAY_CONFIG_DEFAULTBRIGHTNESS;
    // The default for a byte value is 0 if the key is not present, so explicitly check for it.
    if (configDoc["defaultBrightness"].is<JsonVariant>())
    {
        defaultBrightness = configDoc["defaultBrightness"].as<byte>();
    }

    JsonArray displays = configDoc["displays"].as<JsonArray>();
    if (displays.isNull())
    {
        debugPrintln("No displays found in the configuration.");
        return configs;
    }

    for (JsonVariant display : displays)
    {
        DisplayConfiguration config;
        if (tryParseDisplayEntryFromJsonVariant(display, defaultBrightness, config))
        {
            configs->push_back(config);
        }
    }

    return configs;
}

DisplayConfiguration::DisplayConfiguration()
{
}

DisplayConfiguration::DisplayConfiguration(const DisplayConfiguration& other)
{
    copy(other);
}

DisplayConfiguration& DisplayConfiguration::operator=(const DisplayConfiguration& other)
{
    if (this != &other)
    {
        copy(other);
    }
    
    return *this;
}

void DisplayConfiguration::copy(const DisplayConfiguration& other)
{
    this->m_defaultBrightness = other.m_defaultBrightness;
    this->m_gpioPin = other.m_gpioPin;
    this->m_numPixels = other.m_numPixels;
    this->m_numCols = other.m_numCols;
    this->m_numRows = other.m_numRows;
    this->m_colsToLeft = other.m_colsToLeft;
    this->m_colsToRight = other.m_colsToRight;
    this->m_rowsAbove = other.m_rowsAbove;
    this->m_rowsBelow = other.m_rowsBelow;
    this->m_digitsToLeft = other.m_digitsToLeft;
    this->m_digitsToRight = other.m_digitsToRight;
    this->m_digitPixelMapping = other.m_digitPixelMapping;
    this->m_columnPixelMapping = other.m_columnPixelMapping;
    this->m_rowPixelMapping = other.m_rowPixelMapping;
}

bool DisplayConfiguration::tryParseDisplayEntryFromJsonVariant(JsonVariant display, byte defaultBrightness, DisplayConfiguration& config)
{
    if (display.isNull())
    {
        debugPrintln("Display entry was null.");
        return false;
    }

    debugPrint("Parsing display entry: ");
    if (display["name"].is<JsonVariant>())
    {
        const char* name = display["name"].as<const char*>();
        debugPrintln(name);
    }
    else
    {
        debugPrintln("<<no name>>");
    }

    if (display["disabled"].is<JsonVariant>())
    {
        if (display["disabled"].as<bool>())
        {
            debugPrintln("Display entry is disabled.");
            return false;
        }
    }

    if (!display["gpioPin"].is<JsonVariant>())
    {
        debugPrintln("Display entry did not contain a GPIO pin.");
        return false;
    }
    config.m_gpioPin = display["gpioPin"].as<byte>();

    if (!display["numberOfPixels"].is<JsonVariant>())
    {
        debugPrintln("Display entry did not contain the number of pixels.");
        return false;
    }
    config.m_numPixels = display["numberOfPixels"].as<uint16_t>();
    Serial.print("Number of pixels: ");
    Serial.println(config.m_numPixels);

    if (display["defaultBrightness"].is<JsonVariant>())
    {
        config.m_defaultBrightness = display["defaultBrightness"].as<byte>();
    }
    else
    {
        config.m_defaultBrightness = defaultBrightness;
    }

    // The following properties are optional. If not specified, assume the default value of 0.
    config.m_rowsAbove = display["rowsAbove"].as<uint16_t>();
    config.m_rowsBelow = display["rowsBelow"].as<uint16_t>();
    config.m_colsToLeft = display["columnsToLeft"].as<uint16_t>();
    config.m_colsToRight = display["columnsToRight"].as<uint16_t>();
    config.m_digitsToLeft = display["digitsToLeft"].as<uint16_t>();
    config.m_digitsToRight = display["digitsToRight"].as<uint16_t>();

    // Parse the pixel mappings
    if (!display["columnPixelMapping"].is<JsonArray>())
    {
        debugPrintln("Display entry did not contain a valid column pixel mapping.");
        return false;
    }

    JsonArray columnMapping = display["columnPixelMapping"].as<JsonArray>();
    for (int i = 0; i < columnMapping.size(); i++)
    {
        std::vector<uint16_t>* columnPixels = new std::vector<uint16_t>();
        if (!columnMapping[i].is<JsonArray>())
        {
            debugPrintln("Column pixel mapping entry was not an array.");
            return false;
        }

        for (JsonVariant pixel : columnMapping[i].as<JsonArray>())
        {
            columnPixels->push_back(pixel.as<uint16_t>());
        }

        config.m_columnPixelMapping.push_back(columnPixels);
    }

    if (!display["rowPixelMapping"].is<JsonArray>())
    {
        debugPrintln("Display entry did not contain a valid row pixel mapping.");
        return false;
    }

    JsonArray rowMapping = display["rowPixelMapping"].as<JsonArray>();
    for (int i = 0; i < rowMapping.size(); i++)
    {
        std::vector<uint16_t>* rowPixels = new std::vector<uint16_t>();
        if (!rowMapping[i].is<JsonArray>())
        {
            debugPrintln("Row pixel mapping entry was not an array.");
            return false;
        }

        for (JsonVariant pixel : rowMapping[i].as<JsonArray>())
        {
            rowPixels->push_back(pixel.as<uint16_t>());
        }

        config.m_rowPixelMapping.push_back(rowPixels);
    }

    if (!display["digitPixelMapping"].is<JsonArray>())
    {
        // If not defined, put all pixels in a single digit.
        debugPrintln("Display entry did not contain a valid digit pixel mapping. Assuming all pixels are in a single digit.");
        std::vector<uint16_t>* digitPixels = new std::vector<uint16_t>();
        for (int i = 0; i < config.m_numPixels; i++)
        {
            digitPixels->push_back(i);
        }

        config.m_digitPixelMapping.push_back(digitPixels);
    }
    else
    {
        JsonArray digitMapping = display["digitPixelMapping"].as<JsonArray>();
        for (int i = 0; i < digitMapping.size(); i++)
        {
            std::vector<uint16_t>* digitPixels = new std::vector<uint16_t>();
            if (!digitMapping[i].is<JsonArray>())
            {
                debugPrintln("Digit pixel mapping entry was not an array.");
                return false;
            }

            for (JsonVariant pixel : digitMapping[i].as<JsonArray>())
            {
                digitPixels->push_back(pixel.as<uint16_t>());
            }

            config.m_digitPixelMapping.push_back(digitPixels);
        }
    }

    debugPrintln("Display entry parsed successfully.");
    
    return true;
}

void DisplayConfiguration::debugPrint(const char* message)
{
#ifdef PIO_UNIT_TESTING
    printf("%s", message);
#else
    Serial.print(message);
#endif      
}

void DisplayConfiguration::debugPrintln(const char* message)
{
#ifdef PIO_UNIT_TESTING
    printf("%s\n", message);
#else
    Serial.println(message);
#endif      
}