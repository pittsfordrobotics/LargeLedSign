#include <Adafruit_NeoPixel.h>
#include "Arduino.h"
#include "RainbowStyle.h"
#include "PixelBuffer.h"

RainbowStyle::RainbowStyle(String name, PixelBuffer *pixelBuffer) : LightStyle(name, pixelBuffer)
{
  m_nextUpdate = 0;
  m_currentHue = 0;
}

void RainbowStyle::update()
{
  if (millis() < m_nextUpdate)
  {
    return;
  }

  uint32_t newColor = Adafruit_NeoPixel::ColorHSV(m_currentHue);
  shiftColorUsingPattern(newColor);
  incrementHue();
  m_nextUpdate = millis() + getIterationDelay();
}

void RainbowStyle::reset()
{
  m_currentHue = 0;
  int numBlocks = getNumberOfBlocksForPattern();
  int numBlocksToDrain = getNumberOfBlocksToDrain();

  for (int i = 0; i < numBlocksToDrain; i++)
  {
    incrementHue();
  }

  // This loop goes through "numBlocks - 1" because one of the first things done
  // after a reset is to call "update", which will immediately shift it one last time.
  for (int i = 0; i < numBlocks - 1; i++)
  {
    shiftColorUsingPattern(Adafruit_NeoPixel::ColorHSV(m_currentHue));
    incrementHue();
  }

  m_nextUpdate = 0;
}

int RainbowStyle::getIterationDelay()
{
  // Convert "speed" to a delay.
  // Speed ranges from 1 to 100.
  int minDelay = 5;
  int maxDelay = 500;
  double m = (maxDelay - minDelay) / -99.0;
  double b = maxDelay - m;
  int delay = m_speed * m + b;
  return delay;
}

void RainbowStyle::incrementHue()
{
  // Convert "step" to an increment.
  int maxInc = 1000;
  int minInc = 5;
  int maxStep = 100;
  int minStep = 1;
  double m = (double)(maxInc - minInc) / (maxStep - minStep);
  double b = minInc - m;
  int inc = m_step * m + b;
  m_currentHue += inc;
}
