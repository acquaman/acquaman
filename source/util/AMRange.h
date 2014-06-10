#ifndef AMRANGE_H
#define AMRANGE_H

/// The following class is meant to encapsulate a typical range (minimum and maximum).
class AMRange
{
public:
	/// Constructor.  Builds an AMRange from a the \param minimum and \param maximum values provided.
 	virtual ~AMRange();
	AMRange(double minimum, double maximum);
	/// Copy constructor.  Builds a copy AMRange from the one provided.
	AMRange(const AMRange &original);
	/// Null constructor.  Builds a null AMRange.
	AMRange();

	/// Returns whether the range is null.
	bool isNull() const { return isNull_; }
	/// Returns whether the range is invalid.  If range is null, it is also invalid.
	bool isValid() const { return (minimum_ < maximum_) && !isNull_; }
	/// Returns the minimum value of the range.
	double minimum() const { return minimum_; }
	/// Returns the maximum value of the range.
	double maximum() const { return maximum_; }
	/// Returns whether the provided value falls within the limits defined by this range.
	bool withinRange(double value) const { return value >= minimum_ && value <= maximum_; }

	/// Sets the minimum value to \param newValue.
	void setMinimum(double newValue);
	/// Sets the maximum value to \param newValue.
	void setMaximum(double newValue);
	/// Sets the range based on the provided values \param newMinimum and \param newMaximum.
	void setRange(double newMinimum, double newMaximum);

	/// The assignment operator.  Assigns all the values associated with \param other range.
	AMRange &operator =(const AMRange &other);
	/// The equivalence operator.  Checks all the values against \param other's values.
	bool operator ==(const AMRange &other) const;
	/// The not-equivalence operator.  Checks all the values against \param other's values.
	bool operator !=(const AMRange &other) const;

protected:
	/// Flag that holds whether the range is null or not.
	bool isNull_;
	/// The minimum value of the range.
	double minimum_;
	/// The maximum values of the range.
	double maximum_;
};

#endif // AMRANGE_H
