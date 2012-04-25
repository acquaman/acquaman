#ifndef VESPERSBENDINGMIRRORS_H
#define VESPERSBENDINGMIRRORS_H

#include <QWidget>

#include "beamline/CLS/CLSMAXvMotor.h"

/// This is a standalone class that encapsulates the eight motors for moving the M3 and M4 motors.
class VESPERSBendingMirrors : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Builds all the motors and the layout.
	explicit VESPERSBendingMirrors(QWidget *parent = 0);

signals:

public slots:

protected:

	/// Pointer to the M3 pitch control.
	CLSMAXvMotor *m3Pitch_;
	/// Pointer to the M3 Z control.
	CLSMAXvMotor *m3Z_;
	/// Pointer to the M3 upstream bending control.
	CLSMAXvMotor *m3UpstreamBend_;
	/// Pointer to the M3 downstream bending control.
	CLSMAXvMotor *m3DownstreamBend_;
	/// Pointer to the M4 pitch control.
	CLSMAXvMotor *m4Pitch_;
	/// Pointer to the M4 Z control.
	CLSMAXvMotor *m4Z_;
	/// Pointer to the M4 upstream bending control.
	CLSMAXvMotor *m4UpstreamBend_;
	/// Pointer to the M4 downstream bending control.
	CLSMAXvMotor *m4DownstreamBend_;
};

#endif // VESPERSBENDINGMIRRORS_H
