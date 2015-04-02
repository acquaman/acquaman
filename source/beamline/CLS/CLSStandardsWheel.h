#ifndef CLSSTANDARDSWHEEL_H
#define CLSSTANDARDSWHEEL_H

#include <QObject>

#include "beamline/CLS/CLSMAXvMotor.h"

/// This class encapsulates a reference wheel and provides a means of interacting with it using it's concept rather than the raw control.
class CLSStandardsWheel : public QObject
{
	Q_OBJECT

public:
	/// Constructor.  Requires a name and the base PV name for the control.
	explicit CLSStandardsWheel(const QString &name, const QString &basePVName, QObject *parent = 0);
	/// Destructor.
	~CLSStandardsWheel();

signals:

public slots:
	/// Moves the wheel to the specified index.
	void moveToIndex(int index);

protected:
	/// Returns degrees for a given index.
	double indexToDegrees(int index) const;

	/// The control that moves the motor.
	CLSMAXvMotor *wheel_;
};

#endif // CLSSTANDARDSWHEEL_H
