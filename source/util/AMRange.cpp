#include "AMRange.h"

 AMRange::~AMRange(){}
AMRange::AMRange(double minimum, double maximum)
{
	isNull_ = false;
	minimum_ = minimum;
	maximum_ = maximum;
}

AMRange::AMRange(const AMRange &original)
{
	isNull_ = original.isNull();
	minimum_ = original.minimum();
	maximum_ = original.maximum();
}

AMRange::AMRange()
{
	isNull_ = true;
	minimum_ = -1.0;
	maximum_ = -1.0;
}

void AMRange::setRange(double newMinimum, double newMaximum)
{
	isNull_ = false;
	minimum_ = newMinimum;
	maximum_ = newMaximum;
}

void AMRange::setMinimum(double newValue)
{
	isNull_ = false;
	minimum_ = newValue;
}
void AMRange::setMaximum(double newValue)
{
	isNull_ = false;
	maximum_ = newValue;
}

AMRange &AMRange::operator =(const AMRange &other)
{
	isNull_ = other.isNull();
	minimum_ = other.minimum();
	maximum_ = other.maximum();

	return *this;
}

bool AMRange::operator ==(const AMRange &other) const
{
	return (isNull_ == other.isNull())
			&& (minimum_ == other.minimum())
			&& (maximum_ == other.maximum());
}

bool AMRange::operator !=(const AMRange &other) const
{
	return !(this->operator ==(other));
}
