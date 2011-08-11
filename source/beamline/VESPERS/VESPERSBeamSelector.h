#ifndef VESPERSBEAMSELECTOR_H
#define VESPERSBEAMSELECTOR_H

#include <QObject>
#include <QHash>

#include "beamline/CLS/CLSVMEMotor.h"

/*!
  This class changes which beam is being used on the VESPERS beamline.  It creates the link to the motor and then has a look up table of the locations (in steps)
  for the various beams.
  */
class VESPERSBeamSelector : public QObject
{
	Q_OBJECT

public:
	/// Enum for the different beams.
	/*!
		- Pink is not monochromatized and contains all the energies from the bending magnet.
		- TenPercent (10%) is a broad band pass filter.
		- OnePointSixPercent (1.6%) is a narrow band pass filter.
		- Si is the monochromator with 0.01% band pass.
	  */
	enum Beam { None = 0, Pink, TenPercent, OnePointSixPercent, Si };

	/// Constructor.
	explicit VESPERSBeamSelector(QObject *parent = 0);

	/// Returns the current beam in use by the beamline.
	Beam currentBeam() const { return beam_; }

signals:
	/// Notifier that the beam has been changed.
	void currentBeamChanged(Beam);

public slots:
	/// Changes the beam.  Moves the motor to the necessary position.
	void changeBeam(Beam beam);

protected slots:
	/// Determines is currently active on startup.  Also keeps track if the beam is changed outside of Acquaman.  Beam is set to None is if not inside any of the tolerances for the known beam positions.
	void determineBeam();

protected:
	/// Helper function that takes in a desired beam and returns true or false whether the motor is within that beam's tolerance.
	bool isAtBeam(Beam beam) { return (fabs(motor_->step()-positions_.value(beam)) < 1000) ? true : false; }

	/// The current beam in use by the beamline.
	Beam beam_;
	/// Pointer to the motor that controls which beam makes it down the beamline.
	CLSVMEMotor *motor_;

	/// Look up table with the beam and its position.
	QHash<Beam, int> positions_;
};

#endif // VESPERSBEAMSELECTOR_H
