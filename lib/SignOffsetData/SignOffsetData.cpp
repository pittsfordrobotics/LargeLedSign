#include "SignOffsetData.h"

SignOffsetData::SignOffsetData()
{
}

SignOffsetData::SignOffsetData(const SignOffsetData &other)
{
    copy(other);
}

void SignOffsetData::copy(const SignOffsetData &other)
{
    this->digitsToLeft = other.digitsToLeft;
    this->digitsToRight = other.digitsToRight;
    this->columnsToLeft = other.columnsToLeft;
    this->columnsToRight = other.columnsToRight;
}

SignOffsetData &SignOffsetData::operator=(const SignOffsetData &other)
{
    copy(other);
    return *this;
}

bool SignOffsetData::operator==(const SignOffsetData &other)
{
    return (
        this->digitsToLeft == other.digitsToLeft 
        && this->digitsToRight == other.digitsToRight 
        && this->columnsToLeft == other.columnsToLeft 
        && this->columnsToRight == other.columnsToRight);
}

bool SignOffsetData::operator!=(const SignOffsetData &other)
{
    return (
        this->digitsToLeft != other.digitsToLeft 
        || this->digitsToRight != other.digitsToRight 
        || this->columnsToLeft != other.columnsToLeft 
        || this->columnsToRight != other.columnsToRight);
}