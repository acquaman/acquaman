/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMNUMBER_H
#define AMNUMBER_H

/// This is the value returned if you ask an invalid AMNumber for its value.
#define AMNUMBER_INVALID_INTEGER -1
#define AMNUMBER_INVALID_FLOATINGPOINT -1.0

/// This class represents an integer or floating-point value, with additional information on the validity of the value.
/*!  <b>Automatic Conversions</b>

It has conversion operators for ints and doubles, so it can be passed into and used anywhere you would require an int or double.  It also has conversion constructors and operators, allowing an int or double to be passed into any function that requires an AMNumber, and allowing assignment from ints and doubles.

Examples:

<code>
int myInt = 3;
AMNumber myNum;
myNum = myInt;	// myNum is now valid, with type Integer and value 3

double myFloat = myNum;	// myFloat is now 3.0.

// If defined:
void myIntFunction(int a) { ... }
void myDoubleFunction(double b) { ... }

// then can call:
myIntFunction(myNum);
myDoubleFunction(myNum);

// If defined:
void myAMNumFunction(const AMNumber& num) { ... }

// Then can call:
myAMNumFunction(12);	// num inside the function is valid, with type Integer and value 12
myAMNumFunction(2.7); // num inside the function is valid, with type FloatingPoint and value 2.7
myAMNumFunction(myFloat); // num inside the function is valid, with type FloatingPoint and value 3.0
</code>

*/

#include <QString>

class AMNumber
{
public:
	/// Describes the validity of the value.  An un-initialized AMNumber is Null.  An array object could return OutOfBoundsError for attempted access beyond the data it has.
	enum State { Null, Valid, OutOfBoundsError, DimensionError, CalculationError, InvalidError };
	/// The type of the value, as stored natively.  Conversion will always be available.
	enum Type { Integer, FloatingPoint };

	/// Default constructor creates a Null AMNumber
	AMNumber();
	/// Constructs an invalid number with a given State. (\c state should not be Valid, otherwise it will be changed to Null.  This constructor is useful, for ex, in returning an OutOfBoundsError number from an array accessed outside its range.
	AMNumber(State errorState);
	/// Destructor.
	virtual ~AMNumber();

	/// Conversion constructor from int
	AMNumber(int fromInt);
	/// Assignment operator from int
	AMNumber& operator=(int fromInt);
	/// Conversion constructor from double
	AMNumber(double fromDouble);
	/// Assignment operator from double
	AMNumber& operator=(double fromDouble);


	/// Conversion to int:
	operator int() const {
		if(!isValid()) return AMNUMBER_INVALID_INTEGER;
		if(type_ == FloatingPoint) return int(value_.d);
		return value_.i;
	}

	/// Conversion to double:
	operator double() const {
		if(!isValid()) return AMNUMBER_INVALID_FLOATINGPOINT;
		if(type_ == Integer) return double(value_.i);
		return value_.d;
	}

	/// true when the value is valid
	bool isValid() const { return state() == Valid; }
	/// Returns the state (validity) of this number
	State state() const { return state_; }

	/// Returns the type of the number, as stored natively.
	Type type() const { return type_; }

	/// Comparison operator:
	bool operator==(const AMNumber& other) const {
		return state_ == other.state_ && double(*this) == double(other);
	}

	bool operator==(double d) const {
		return state_ == Valid && double(*this) == d;
	}

	bool operator!=(const AMNumber &other) const{
		return !operator==(other);
	}

	bool operator!=(double d) const{
		return !operator==(d);
	}

	/// Print as a string
	QString toString(QChar format = 'g', int precision = 19);

	QString toString(int precision);

protected:
	State state_;
	Type type_;
	union { int i; double d; } value_;


};


#endif // AMNUMBER_H
