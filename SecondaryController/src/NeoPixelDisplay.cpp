#include "NeoPixelDisplay.h"

NeoPixelDisplay::NeoPixelDisplay(const DisplayConfiguration* displayConfiguration)
{
    m_pixelBuffer = new PixelBuffer(displayConfiguration);
    m_neoPixels = new Adafruit_NeoPixel(m_pixelBuffer->getPixelCount(), displayConfiguration->getGpioPin(), NEO_GRB + NEO_KHZ800);
    m_neoPixels->begin();
    m_neoPixels->clear();

    // TODO: read default brightness from config and set neoPixels brightness.
}

void NeoPixelDisplay::setBrightness(byte brightness)
{
    m_neoPixels->setBrightness(brightness);
}

void NeoPixelDisplay::updateDisplay()
{
    bool wasUpdated = false;

    if (m_displayPattern)
    {
        wasUpdated = m_displayPattern->update(m_pixelBuffer);
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
        m_displayPattern->reset(m_pixelBuffer);
    }
    
    outputPixels();
}

void NeoPixelDisplay::outputPixels()
{
    for (uint i = 0; i < m_pixelBuffer->getPixelCount(); i++)
    {
        m_neoPixels->setPixelColor(i, m_pixelBuffer->getPixel(i));
    }

    unsigned long start = millis();
    m_neoPixels->show();

    // I have no idea why, but if we exit immediately and try to read BLE settings,
    // the BLE readings are sometimes corrupt.  If we wait until the "show" is done
    // and delay a tiny bit more, things are stable.
    while (!m_neoPixels->canShow())
    {
        // wait for the "show" to complete
    }
    while (millis() - start < 10)
    {
        // wait until at least 10 msec have passed since starting the "show"
    }
}

