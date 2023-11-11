#include "Arduino.h"
#include "LightStyle.h"
#include "PixelBuffer.h"

std::vector<String> LightStyle::knownPatterns;

LightStyle::LightStyle(String name, PixelBuffer* pixelBuffer) {
  m_pixelBuffer = pixelBuffer;
  m_name = name;
  if (knownPatterns.size() == 0) {
    knownPatterns.push_back("Solid");
    knownPatterns.push_back("Right");
    knownPatterns.push_back("Left");
    knownPatterns.push_back("Up");
    knownPatterns.push_back("Down");
    knownPatterns.push_back("Digit");
    knownPatterns.push_back("Random");
  }
}

void LightStyle::setSpeed(uint8_t speed) {
  m_speed = speed;
}

void LightStyle::setStep(byte step) {
  m_step = step;
}

String LightStyle::getName() {
  return m_name;
}

void LightStyle::setPattern(byte pattern) {
  m_pattern = pattern;
}

int LightStyle::getNumberOfBlocksForPattern() {
  switch (m_pattern) {
    case 1: // Right
    case 2: // Left
      return m_pixelBuffer->getColumnCount();
    case 3: // Up
    case 4: // Down
      return m_pixelBuffer->getRowCount();
    case 5: // Digit
      //return m_pixelBuffer->getDigitCount(); // No more digits - just solid.
      return 1;
    case 6: // "Random" (linear)
      return m_pixelBuffer->getPixelCount();
    default: // 0 = solid
      // Default to Solid (ie, all lights the same color)
      return 1;
  }
}

void LightStyle::shiftColorUsingPattern(uint32_t newColor) {
  // Stick with integer values here instead of doing a bunch
  // of "ifs" to compare strings.  
  switch (m_pattern) {
    case 1: // Right
      m_pixelBuffer->shiftColumnsRight(newColor);
      return;
    case 2: // Left
      m_pixelBuffer->shiftColumnsLeft(newColor);
      return;
    case 3: // Up
      m_pixelBuffer->shiftRowsUp(newColor);
      return;
    case 4: // Down
      m_pixelBuffer->shiftRowsDown(newColor);
      return;
    case 5: // Digit -- no more digit, just use solid for now.
      //m_pixelBuffer->shiftDigitsRight(newColor);
      for (uint i = 0; i < m_pixelBuffer->getPixelCount(); i++) {
        m_pixelBuffer->setPixel(i, newColor);
      }
      return;
    case 6: // "Random" - really linear, but appears sort of random on the real sign
      m_pixelBuffer->shiftLineRight(newColor);
      return;
    default:
      // Default to Solid (ie, all lights the same color)
      for (uint i = 0; i < m_pixelBuffer->getPixelCount(); i++) {
        m_pixelBuffer->setPixel(i, newColor);
      }
  }
}

