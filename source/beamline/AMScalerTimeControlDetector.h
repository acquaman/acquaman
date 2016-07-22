#ifndef AMSCALERTIMECONTROLDETECTOR_H
#define AMSCALERTIMECONTROLDETECTOR_H

#include "beamline/AMBasicControlDetectorEmulator.h"

/// This class is a simple extension of the basic control detector emulator specifically for the scaler time.
/*!
	The reason for this extention is because everything time based is in seconds throughout all of the equipment
	and also everywhere in AM.  Unfortunately, the scaler has it's time in milliseconds and if it was to be
	added into a scan, it would require a scaling by one thousand and that would add unnecessary clutter to
	the scan data.
  */
class AMScalerTimeControlDetector : public AMBasicControlDetectorEmulator
{
	Q_OBJECT

public:
	/// Constructor.  Pass in the control that encapsulates the scaler time.
	AMScalerTimeControlDetector(const QString &name, const QString &description, AMControl *scalerTime, QObject *parent = 0);

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or if the data is not ready.
	virtual AMNumber reading(const AMnDIndex& indexes) const;
	/// Fills the given double pointer with our current value
	virtual bool data(double *outputValues) const;
};

#endif // AMSCALERTIMECONTROLDETECTOR_H
