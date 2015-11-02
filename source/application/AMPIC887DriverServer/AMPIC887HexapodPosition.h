#ifndef AMPIC887HEXAPODPOSITION_H
#define AMPIC887HEXAPODPOSITION_H

#include <QString>
/*!
  * A class which represents the position of all six axes of a hexapod at a given
  * point in time.
  */
class AMPIC887HexapodPosition
{
public:
	/*!
	  * Creates an invalid hexapod position. All values will be set to zero, and
	  * calling isValid() will return false.
	  */
	AMPIC887HexapodPosition();

	/*!
	  * Creates an instance of a hexapod position which represents the provided
	  * position/time combination.
	  */
	AMPIC887HexapodPosition(double xAxisPosition,
							double yAxisPosition,
							double zAxisPosition,
							double uAxisPosition,
							double vAxisPosition,
							double wAxisPosition,
							double time);

	/// The position of the x axis
	double xAxisPosition() const;

	/// The position of the y axis
	double yAxisPosition() const;

	/// The position of the z axis
	double zAxisPosition() const;

	/// The position of the u axis
	double uAxisPosition() const;

	/// The position of the v axis
	double vAxisPosition() const;

	/// The position of the w axis
	double wAxisPosition() const;	

	/// The time after motion commenced that the hexapod was in this position (ms)
	double time() const;

	/// A string representation of the hexapod position
	QString toString() const;

	/// Whether the hexapod position is valid or not. Returns true if the hexapod
	/// position was constructed with values.
	bool isValid() const;
protected:
	double xAxisPosition_;
	double yAxisPosition_;
	double zAxisPosition_;
	double uAxisPosition_;
	double vAxisPosition_;
	double wAxisPosition_;
	double time_;
	bool isValid_;
};

#endif // AMPIC887HEXAPODPOSITION_H
