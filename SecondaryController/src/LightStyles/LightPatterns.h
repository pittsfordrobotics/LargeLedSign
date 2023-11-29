#ifndef LIGHTPATTERNS_H
#define LIGHTPATTERNS_H

enum class LightPatterns {
    Solid = 0,
    Right = 1,
    Left = 2,
    Up = 3,
    Down = 4,
    Digit = 5
};

// This list of names needs to be kept in sync with the enum values.
#define LIGHTPATTERN_NAMES "Solid", "Right", "Left", "Up", "Down", "Digit" 

#endif