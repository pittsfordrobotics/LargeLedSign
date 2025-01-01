#include "BluetoothConfig.h"

BluetoothConfig::BluetoothConfig()
{
}

BluetoothConfig* BluetoothConfig::ParseJson(const char* jsonString, size_t length)
{
    BluetoothConfig* config = new BluetoothConfig();

    JsonDocument configDoc;
    DeserializationError err = deserializeJson(configDoc, jsonString, length);
    
    if (err == DeserializationError::EmptyInput)
    {
        Serial.println("Bluetooth configuration JSON string was empty.");
        return config;
    }

    if (err != DeserializationError::Ok)
    {
        Serial.print("Error parsing Bluetooth configuration JSON: ");
        Serial.println(err.c_str());
        return config;
    }

    if (configDoc.isNull())
    {
        Serial.println("Bluetooth configuration JSON was null after parsing.");
        return config;
    }

    if (configDoc["uuid"].is<JsonVariant>())
    {
        config->m_uuid = configDoc["uuid"].as<String>();
    }

    if (configDoc["localName"].is<JsonVariant>())
    {
        config->m_localName = configDoc["localName"].as<String>();
    }

    return config;
}