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
			if (type.equalsIgnoreCase("blank") || type.equalsIgnoreCase(String((int)ColorPatternType::Blank)))
			{
				return ColorPatternType::Blank;
			}
			if (type.equalsIgnoreCase("singlecolor") || type.equalsIgnoreCase(String((int)ColorPatternType::SingleColor)))
			{
				return ColorPatternType::SingleColor;
			}
			if (type.equalsIgnoreCase("twocolor") || type.equalsIgnoreCase(String((int)ColorPatternType::TwoColor)))
			{
				return ColorPatternType::TwoColor;
			}
			if (type.equalsIgnoreCase("rainbow") || type.equalsIgnoreCase(String((int)ColorPatternType::Rainbow)))
			{
				return ColorPatternType::Rainbow;
			}
			if (type.equalsIgnoreCase("twocolorfade") || type.equalsIgnoreCase(String((int)ColorPatternType::TwoColorFade)))
			{
				return ColorPatternType::TwoColorFade;
			}
			if (type.equalsIgnoreCase("threecolorfade") || type.equalsIgnoreCase(String((int)ColorPatternType::ThreeColorFade)))
			{
				return ColorPatternType::ThreeColorFade;
			}
			if (type.equalsIgnoreCase("fourcolorfade") || type.equalsIgnoreCase(String((int)ColorPatternType::FourColorFade)))
			{
				return ColorPatternType::FourColorFade;
			}
			if (type.equalsIgnoreCase("backgroundplusthree") || type.equalsIgnoreCase(String((int)ColorPatternType::BackgroundPlusThree)))
			{
				return ColorPatternType::BackgroundPlusThree;
			}
			return ColorPatternType::Blank;
		}
};

#endif