#ifndef PATTERNDATA_H
#define PATTERNDATA_H

#include <Arduino.h>
#include "ColorPatternType.h"
#include "DisplayPatternType.h"

// The max data packet we can send from the primary controller to the
// secondary controller is 20 bytes, so make sure we're under that limit.
struct PatternData {
	public:
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

		// Since we're using RGB LEDs with no White value, utilize the empty
		// White value in each of the colors below for more parameters.

		// The first color in the color pattern, if needed for the pattern.
		union {
			ulong color1{0};
			struct {
				byte blue1;
				byte green1;
				byte red1;
				byte param3;
			};
		};

		// The second color in the color pattern, if needed for the pattern.
		union {
			ulong color2{0};
			struct {
				byte blue2;
				byte green2;
				byte red2;
				byte param4;
			};
		};

		// The third color in the color pattern, if needed for the pattern.
		union {
			ulong color3{0};
			struct {
				byte blue3;
				byte green3;
				byte red3;
				byte param5;
			};
		};

		// The fourth color in the color pattern, if needed for the pattern.
		union {
			ulong color4{0};
			struct {
				byte blue4;
				byte green4;
				byte red4;
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

	private:
		// Copies all struct member values from the provided PatternData.
		void copy(const PatternData& other);
};

#endif