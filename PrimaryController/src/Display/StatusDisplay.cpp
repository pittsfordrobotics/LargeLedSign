#include "StatusDisplay.h"

byte Dot{0b10000000};

StatusDisplay::StatusDisplay(byte clockPin, byte dioPin, byte brightness)
{
    m_display = new TM1637Display(clockPin, dioPin);
    m_display->clear();
    m_display->setBrightness(brightness);
}

StatusDisplay::~StatusDisplay()
{
    delete m_display;
}

void StatusDisplay::update()
{
    if (m_currentPriority == DisplayPriority::None || m_currentPriority == DisplayPriority::AdHoc)
    {
        // If None, nothing to do.  If AdHoc, leave it as it is.
        return;
    }

    if (millis() > m_nextUpdate)
    {
        if (m_currentPriority == DisplayPriority::Ephemeral)
        {
            m_display->clear();
            m_currentPriority = DisplayPriority::None;
        }
        else if (m_currentPriority == DisplayPriority::Sequence)
        {
            if (!m_displayQueue.empty())
            {
                displayString(m_displayQueue.front());
                m_displayQueue.pop();
                m_nextUpdate = millis() + m_sequenceDuration;
            }
            else
            {
                m_display->clear();
                m_currentPriority = DisplayPriority::None;
            }
        }
    }
}

void StatusDisplay::clear() 
{
    m_display->clear();
    m_currentPriority = DisplayPriority::None;
}

// String should be 4 chars, plus any dots.
// Ex: "A=1.2"
// The dot will be applied to the prior segment.
// To display just a dot for a digit, use a space first.
// Ex: to display "....", the string should be " . . . ."
// Custom characters: (TBD)
void StatusDisplay::setDisplay(String stringToDisplay)
{
    // Adhoc is the highest priority, so don't bother checking the current priority.
    displayString(stringToDisplay);
    m_currentPriority = DisplayPriority::AdHoc;
}

void StatusDisplay::displayTemporary(String stringToDisplay, uint durationMsec)
{
    if (m_currentPriority > DisplayPriority::Ephemeral)
    {
        return;
    }

    displayString(stringToDisplay);
    m_nextUpdate = millis() + durationMsec;
    m_currentPriority = DisplayPriority::Ephemeral;
}

void StatusDisplay::displaySequence(std::vector<String> stringsToDisplay, uint durationMsec)
{
    if (m_currentPriority > DisplayPriority::Sequence)
    {
        return;
    }

    // Reset any current sequence
    while (!m_displayQueue.empty())
    {
        m_displayQueue.pop();
    }

    if (stringsToDisplay.size() == 0)
    {
        // Nothing to do
        return;
    }

    // Load the first string immediately
    displayString(stringsToDisplay[0]);
    m_sequenceDuration = durationMsec;
    m_nextUpdate = millis() + m_sequenceDuration;

    // Load any remaining strings into the queue.
    for (uint i = 1; i < stringsToDisplay.size(); i++)
    {
        m_displayQueue.push(stringsToDisplay[i]);
    }

    m_currentPriority = DisplayPriority::Sequence;
}

void StatusDisplay::displayString(String s)
{
    byte buffer[4] {0, 0, 0, 0};

    // Convert characters until we run out or hit 4 characters.
    uint stringPosition = 0;
    uint bufferPosition = 0;

    while (stringPosition < s.length() && bufferPosition < 4) {
        char c = s.charAt(stringPosition);
        buffer[bufferPosition] = convertCharacter(c);
        bufferPosition++;
        stringPosition++;
        if (stringPosition >= s.length())
        {
            // no more characters
            break;
        }

        // Peek at the next character to see if it's a dot.
        c = s.charAt(stringPosition);
        if (c == '.')
        {
            // set the dot on the prior digit
            buffer[bufferPosition - 1] =  buffer[bufferPosition - 1] | Dot;
            stringPosition++;
        }
    }

    m_display->setSegments(buffer);
}

byte StatusDisplay::convertCharacter(char c)
{
    switch (c) {
        case '0':
            return m_display->encodeDigit(0);
        case '1':
            return m_display->encodeDigit(1);
        case '2':
            return m_display->encodeDigit(2);
        case '3':
            return m_display->encodeDigit(3);
        case '4':
            return m_display->encodeDigit(4);
        case '5':
            return m_display->encodeDigit(5);
        case '6':
            return m_display->encodeDigit(6);
        case '7':
            return m_display->encodeDigit(7);
        case '8':
            return m_display->encodeDigit(8);
        case '9':
            return m_display->encodeDigit(9);
        case 'A':
        case 'a':
            return m_display->encodeDigit(10);
        case 'B':
        case 'b':
            return m_display->encodeDigit(11);
        case 'C':
        case 'c':
            return m_display->encodeDigit(12);
        case 'D':
        case 'd':
            return m_display->encodeDigit(13);
        case 'E':
        case 'e':
            return m_display->encodeDigit(14);
        case 'F':
        case 'f':
            return m_display->encodeDigit(15);
        case '-':
            return 0b01000000;
        case ' ':
            return 0;
        case '=':
            return 0b01001000;
        default:
            return 0;
    }
}
