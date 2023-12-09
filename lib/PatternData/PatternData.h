#ifndef PATTERNDATA_H
#define PATTERNDATA_H

#include <Arduino.h>

enum class ColorPatternType : byte
{
	Blank = 0,
	SingleColor = 1,
	TwoColor = 2,
	Rainbow = 3
};

enum class DisplayPatternType : byte
{
    Solid = 0,
    Right = 1,
    Left = 2,
    Up = 3,
    Down = 4,
    Digit = 5,
    Random = 6
};

// The max data packet we can send from the primary controller to the
// secondary controller is 20 bytes, so make sure we're under that limit.
struct PatternData {
	// Indicates the color pattern.
	// Ex: Single color, 2-color, rainbow, etc.
	ColorPatternType colorPattern{ColorPatternType::Blank};

	// Indicates the pattern to use to display the selected color(s).
	// Ex: Shift by columns right/left, Shift by rows up/down, fill randomly, etc.
	DisplayPatternType displayPattern{DisplayPatternType::Solid};

	// Optional parameter for the pattern.
	// Ex: The color increment for the rainbow pattern.
	byte param1{0};

	// Optional parameter for the pattern.
	byte param2{0};

	// The first color in the color pattern, if needed for the pattern.
	ulong color1{0};

	// The second color in the color pattern, if needed for the pattern.
	ulong color2{0};

	// The third color in the color pattern, if needed for the pattern.
	ulong color3{0};

	// The fourth color in the color pattern, if needed for the pattern.
	// --OR --
	// Four additional optional parameters used to control the pattern.
	union {
		ulong color4{0};
		struct {
			byte param3;
			byte param4;
			byte param5;
			byte param6;
		};
	};

	// Creates a new PatternData struct with default values.
	PatternData();

	// Creates a new PatternData struct with values copied from the provided PatternData.
	PatternData(const PatternData& other);

	// Sets the values of the current PatternData to be the same as the provided PatternData.
    PatternData& operator=(const PatternData& other);

	// Returns true if this PatternData is the same as the provided PatternData.
    bool operator==(const PatternData& other);

	// Returns true if this PatternData is different than the provided PatternData.
    bool operator!=(const PatternData& other);
};

#endif