#include "NeoPixelDisplay.h"

NeoPixelDisplay::NeoPixelDisplay(const DisplayConfiguration& displayConfiguration)
{
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
    m_displayPattern = displayPattern;
    m_displayPattern->reset(m_pixelMap);
}

void NeoPixelDisplay::updateDisplay()
{
    bool wasUpdated = false;

    if (m_displayPattern)
    {
        wasUpdated = m_displayPattern->update(m_pixelMap);
    }

    if (wasUpdated)
    {
        outputPixels();
    }
}

void NeoPixelDisplay::resetDisplay()
{
    if (m_displayPattern)
    {
        m_displayPattern->reset(m_pixelMap);
    }
    
    outputPixels();
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

