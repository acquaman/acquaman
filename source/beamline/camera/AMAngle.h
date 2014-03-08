#ifndef AMANGLE_H
#define AMANGLE_H

#include <QObject>


/// AMAngle represents an angle
/// automatically converts when
/// needed and forces accesses to
/// request type (degrees, radians, ...)

class AMAngle : public QObject
{
	Q_OBJECT
public:
	enum angleState {DEG, RAD};
	explicit AMAngle(QObject *parent = 0);
	AMAngle(double value, angleState state = DEG, QObject *parent = 0);

	double radians();
	double degrees();

	/// conversion functions
	double radToDeg(double angle);
	double degToRad(double angle);
	
signals:
	
public slots:
	/// set angle in radians
	void setRadians(double angle);
	/// set angle in pi radians (specify multiple of pi)
	void setPiRadians(double nAngle);
	/// set angle in degrees
	void setDegrees(double angle);
	/// set angle from another AMAngle
	void setAngle(AMAngle &angle);

protected:
	angleState  state();
	double angle();

	void setState(angleState state);
	void setAngle(double angle);


protected:

	double angle_;
	angleState state_;
	
};

#endif // AMANGLE_H
