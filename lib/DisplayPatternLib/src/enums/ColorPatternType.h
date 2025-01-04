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

class ColorPatternTypeHelper
{
	public:
		static ColorPatternType fromString(String type)
		{
			if (type.equalsIgnoreCase("blank"))
			{
				return ColorPatternType::Blank;
			}
			if (type.equalsIgnoreCase("singlecolor"))
			{
				return ColorPatternType::SingleColor;
			}
			if (type.equalsIgnoreCase("twocolor"))
			{
				return ColorPatternType::TwoColor;
			}
			if (type.equalsIgnoreCase("rainbow"))
			{
				return ColorPatternType::Rainbow;
			}
			if (type.equalsIgnoreCase("twocolorfade"))
			{
				return ColorPatternType::TwoColorFade;
			}
			if (type.equalsIgnoreCase("threecolorfade"))
			{
				return ColorPatternType::ThreeColorFade;
			}
			if (type.equalsIgnoreCase("fourcolorfade"))
			{
				return ColorPatternType::FourColorFade;
			}
			if (type.equalsIgnoreCase("backgroundplusthree"))
			{
				return ColorPatternType::BackgroundPlusThree;
			}
			return ColorPatternType::Blank;
		}
};

#endif