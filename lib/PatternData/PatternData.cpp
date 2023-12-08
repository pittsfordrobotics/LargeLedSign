#include "PatternData.h"

PatternData::PatternData()
{
}

PatternData::PatternData(const PatternData& other)
{
    this->colorPattern = other.colorPattern;
    this->displayPattern = other.displayPattern;
    this->param1 = other.param1;
    this->param2 = other.param2;
    this->color1 = other.color1;
    this->color2 = other.color2;
    this->color3 = other.color3;
    this->color4 = other.color4;
}

PatternData& PatternData::operator=(const PatternData& other)
{
    this->colorPattern = other.colorPattern;
    this->displayPattern = other.displayPattern;
    this->param1 = other.param1;
    this->param2 = other.param2;
    this->color1 = other.color1;
    this->color2 = other.color2;
    this->color3 = other.color3;
    this->color4 = other.color4;

    return *this;
}

bool PatternData::operator==(const PatternData& other)
{
    return (this->colorPattern == other.colorPattern
        && this->displayPattern == other.displayPattern
        && this->param1 == other.param1
        && this->param2 == other.param2
        && this->color1 == other.color1
        && this->color2 == other.color2
        && this->color3 == other.color3
        && this->color4 == other.color4);
}

bool PatternData::operator!=(const PatternData& other)
{
    return (this->colorPattern != other.colorPattern
        || this->displayPattern != other.displayPattern
        || this->param1 != other.param1
        || this->param2 != other.param2
        || this->color1 != other.color1
        || this->color2 != other.color2
        || this->color3 != other.color3
        || this->color4 != other.color4);
}
