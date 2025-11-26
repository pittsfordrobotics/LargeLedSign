#include "NeoPixelDisplay.h"

NeoPixelDisplay::NeoPixelDisplay(const DisplayConfiguration& displayConfiguration)
{
    mutex_init(&m_lockObject);
    m_displayPattern = nullptr;
    m_pixelMap = new PixelMap(displayConfiguration);
    m_neoPixels = new Adafruit_NeoPixel(m_pixelMap->getPixelCount(), displayConfiguration.getGpioPin(), NEO_GRB + NEO_KHZ800);
    m_neoPixels->begin();
    m_neoPixels->clear();
    m_neoPixels->setBrightness(displayConfiguration.getDefaultBrightness());
}

void NeoPixelDisplay::setBrightness(byte brightness)
{
    m_neoPixels->setBrightness(brightness);
}

byte NeoPixelDisplay::getBrightness()
{
    return m_neoPixels->getBrightness();
}

void NeoPixelDisplay::setDisplayPattern(DisplayPattern* displayPattern)
{
    Serial.println("Setting new display pattern.");
    //mutex_enter_blocking(&m_lockObject);
    DisplayPattern* oldPattern = m_displayPattern;
    m_displayPattern = displayPattern;
    Serial.println("Resetting display pattern.");
    m_displayPattern->reset(m_pixelMap);
    Serial.println("Display pattern reset.");
    //mutex_exit(&m_lockObject);
    
    // Delete old pattern AFTER releasing lock to avoid holding lock during delete
    if (oldPattern) {
        Serial.println("Deleting old display pattern.");
        delete oldPattern;
    }
    Serial.println("New display pattern set.");
}

void NeoPixelDisplay::updateDisplay()
{
    //mutex_enter_blocking(&m_lockObject);
    bool wasUpdated = false;

    if (m_displayPattern)
    {
        wasUpdated = m_displayPattern->update(m_pixelMap);
    }

    if (wasUpdated)
    {
        outputPixels();
    }
    //mutex_exit(&m_lockObject);
}

void NeoPixelDisplay::resetDisplay()
{
    //mutex_enter_blocking(&m_lockObject);
    if (m_displayPattern)
    {
        m_displayPattern->reset(m_pixelMap);
    }
    
    outputPixels();
    //mutex_exit(&m_lockObject);
}

void NeoPixelDisplay::setSpeed(byte speed)
{
    //mutex_enter_blocking(&m_lockObject);
    if (m_displayPattern)
    {
        m_displayPattern->setSpeed(speed);
    }
    //mutex_exit(&m_lockObject);
}

void NeoPixelDisplay::outputPixels()
{
    for (uint i = 0; i < m_pixelMap->getPixelCount(); i++)
    {
        m_neoPixels->setPixelColor(i, m_pixelMap->getPixel(i));
    }

    unsigned long start = millis();
    m_neoPixels->show();
    
    while (!m_neoPixels->canShow())
    {
        // wait for the "show" to complete
    }
}

