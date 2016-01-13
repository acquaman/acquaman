#ifndef BIOXAS32ELEMENTGEDETECTOR_H
#define BIOXAS32ELEMENTGEDETECTOR_H

#include "beamline/AMXspress3XRFDetector.h"
#include "application/BioXAS/BioXAS.h"

/// Implementation of AMXRFDetector for the 32-element Germanium detector on BioXAS.
class BioXAS32ElementGeDetector : public AMXspress3XRFDetector
{
	Q_OBJECT

public:
	/// Default constructor.  Requires the name and description of the detector, as well as the necessary acquire control and setting dwell time control. It builds all the PV's and connects them accordingly.
	BioXAS32ElementGeDetector(const QString &name, const QString &description, AMPVControl *acquireControl, AMPVControl *dwellTimeControl, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXAS32ElementGeDetector();

	/// Returns the type of the detector
	virtual int type() { return BioXAS::Ge13ElementDetector; }
};

#endif // BIOXAS32ELEMENTGEDETECTOR_H
