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
    mutex_enter_blocking(&m_lockObject);
    DisplayPattern* oldPattern = m_displayPattern;
    m_displayPattern = displayPattern;
    m_displayPattern->reset(m_pixelMap);
    mutex_exit(&m_lockObject);
    
    if (oldPattern) {
        delete oldPattern;
    }
}

uint16_t NeoPixelDisplay::getPixelCount()
{
    if (m_pixelMap == nullptr) 
    {
        return 0;
    }

    return m_pixelMap->getPixelCount();
}

uint16_t NeoPixelDisplay::getColumnCount()
{
    if (m_pixelMap == nullptr) 
    {
        return 0;
    }

    return m_pixelMap->getColumnCount();
}

uint16_t NeoPixelDisplay::getRowCount()
{
    if (m_pixelMap == nullptr) 
    {
        return 0;
    }

    return m_pixelMap->getRowCount();
}

uint16_t NeoPixelDisplay::getDigitCount()
{
    if (m_pixelMap == nullptr) 
    {
        return 0;
    }

    return m_pixelMap->getDigitCount();
}

void NeoPixelDisplay::setDigitsToLeft(uint16_t digitsToLeft)
{
    m_pixelMap->setDigitsToLeft(digitsToLeft);
}

void NeoPixelDisplay::setDigitsToRight(uint16_t digitsToRight)
{
    m_pixelMap->setDigitsToRight(digitsToRight);
}

void NeoPixelDisplay::setColumnsToLeft(uint16_t colsToLeft)
{
    m_pixelMap->setColsToLeft(colsToLeft);
}

void NeoPixelDisplay::setColumnsToRight(uint16_t colsToRight)
{
    m_pixelMap->setColsToRight(colsToRight);
}

void NeoPixelDisplay::updateDisplay()
{
    mutex_enter_blocking(&m_lockObject);
    bool wasUpdated = false;

    if (m_displayPattern)
    {
        wasUpdated = m_displayPattern->update(m_pixelMap);
    }

    if (wasUpdated)
    {
        outputPixels();
    }
    mutex_exit(&m_lockObject);
}

void NeoPixelDisplay::resetDisplay()
{
    mutex_enter_blocking(&m_lockObject);
    if (m_displayPattern)
    {
        m_displayPattern->reset(m_pixelMap);
    }
    
    outputPixels();
    mutex_exit(&m_lockObject);
}

void NeoPixelDisplay::setSpeed(byte speed)
{
    mutex_enter_blocking(&m_lockObject);
    if (m_displayPattern)
    {
        m_displayPattern->setSpeed(speed);
    }
    mutex_exit(&m_lockObject);
}

void NeoPixelDisplay::outputPixels()
{
    uint32_t startTime = millis();

    // No locking here:
    // This is an internal method and we should have already taken a lock.
    for (uint i = 0; i < m_pixelMap->getPixelCount(); i++)
    {
        m_neoPixels->setPixelColor(i, m_pixelMap->getRawPixelColor(i));
    }

    m_neoPixels->show();
    
    while (!m_neoPixels->canShow())
    {
        // wait for the "show" to complete
    }

    m_totalTimeUpdatingPixels += (millis() - startTime);
    m_numberOfPixelUpdates++;
}

void NeoPixelDisplay::resetTelemetry()
{
    m_numberOfPixelUpdates = 0;
    m_totalTimeUpdatingPixels = 0;

    if (m_displayPattern)
    {
        m_displayPattern->resetTelemetry();
    }
}

double NeoPixelDisplay::getAverageTimeUpdatingLeds()
{
    if (m_numberOfPixelUpdates == 0)
    {
        return 0;
    }

    return (double) m_totalTimeUpdatingPixels / m_numberOfPixelUpdates;
}

double NeoPixelDisplay::getAverageTimeUpdatingPixelMap()
{
    if (m_displayPattern)
    {
        return m_displayPattern->getAverageTimeUpdatingPixelMap();
    }

    return 0;
}

uint16_t NeoPixelDisplay::getDroppedFrames()
{
    if (m_displayPattern)
    {
        return m_displayPattern->getDroppedFrames();
    }

    return 0;
}


