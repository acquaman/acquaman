#ifndef BIOXASDBHRMIRRORS_H
#define BIOXASDBHRMIRRORS_H

#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/BioXAS/BioXASBeamlineComponent.h"

class BioXASDBHRMirrors : public BioXASBeamlineComponent
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASDBHRMirrors(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASDBHRMirrors();

	/// Returns true if the mirrors are connected, false otherwise.
	bool isConnected() const;

	/// Returns the m1 vertical motor control.
	CLSMAXvMotor* m1VerticalControl() const { return m1Vertical_; }
	/// Returns the m2 vertical motor control.
	CLSMAXvMotor* m2VerticalControl() const { return m2Vertical_; }
	/// Returns the pitch motor control.
	CLSMAXvMotor* pitchControl() const { return pitch_; }

protected:
	/// The m1 vertical motor.
	CLSMAXvMotor *m1Vertical_;
	/// The m2 vertical motor.
	CLSMAXvMotor *m2Vertical_;
	/// The pitch motor.
	CLSMAXvMotor *pitch_;
};

#endif // BIOXASDBHRMIRRORS_H
