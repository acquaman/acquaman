#ifndef VESPERSBEAMSELECTOR_H
#define VESPERSBEAMSELECTOR_H

#include <QObject>

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
	enum Beam { Pink = 0, TenPercent, OnePointSixPercent, Si };

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

protected:
	/// The current beam in use by the beamline.
	Beam beam_;
	/// Pointer to the motor that controls which beam makes it down the beamline.
	CLSVMEMotor *motor_;
};

#endif // VESPERSBEAMSELECTOR_H
