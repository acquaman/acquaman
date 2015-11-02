#ifndef AMPIC887HEXAPODPOSITION_H
#define AMPIC887HEXAPODPOSITION_H

#include <QString>
class AMPIC887HexapodPosition
{
public:
	AMPIC887HexapodPosition();

	AMPIC887HexapodPosition(double xAxisPosition,
							double yAxisPosition,
							double zAxisPosition,
							double uAxisPosition,
							double vAxisPosition,
							double wAxisPosition,
							double time);

	double xAxisPosition() const;

	double yAxisPosition() const;

	double zAxisPosition() const;

	double uAxisPosition() const;

	double vAxisPosition() const;

	double wAxisPosition() const;

	double time() const;

	QString toString() const;
protected:
	double xAxisPosition_;
	double yAxisPosition_;
	double zAxisPosition_;
	double uAxisPosition_;
	double vAxisPosition_;
	double wAxisPosition_;
	double time_;
};

#endif // AMPIC887HEXAPODPOSITION_H
