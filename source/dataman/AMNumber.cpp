#include "AMNumber.h"

/// Default constructor creates a Null AMNumber
AMNumber::AMNumber() {
	type_ = Integer;
	state_ = Null;
}

/// Constructs an invalid number with a given State. (\c state should not be Valid, otherwise it will be changed to Null.  This constructor is useful, for ex, in returning an OutOfBoundsError number from an array accessed outside its range.
AMNumber::AMNumber(State errorState) {
	state_ = errorState;
	if(state_ == Valid)
		state_ = Null;
	type_ = Integer;
}

/// Conversion constructor from int
AMNumber::AMNumber(int fromInt) {
	type_ = Integer;
	value_.i = fromInt;
	state_ = Valid;
}

/// Conversion constructor from double
AMNumber::AMNumber(double fromDouble) {
	type_ = FloatingPoint;
	value_.d = fromDouble;
	state_ = Valid;
}

/// Assignment operator from double
AMNumber& AMNumber::operator=(double fromDouble) {
	type_ = FloatingPoint;
	value_.d = fromDouble;
	state_ = Valid;

	return *this;
}

/// Assignment operator from int
AMNumber& AMNumber::operator=(int fromInt) {
	type_ = Integer;
	value_.i = fromInt;
	state_ = Valid;

	return *this;
}
