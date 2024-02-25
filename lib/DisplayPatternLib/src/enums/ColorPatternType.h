#ifndef COLORPATTERNTYPE_H
#define COLORPATTERNTYPE_H

#include <Arduino.h>

enum class ColorPatternType : byte
{
	Blank = 0,
	SingleColor = 1,
	TwoColor = 2,
	Rainbow = 3,
	TwoColorFade = 4,
	ThreeColorFade = 5,
	FourColorFade = 6,
	BackgroundPlusThree = 7
};

#endif