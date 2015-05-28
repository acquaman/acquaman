#ifndef BIOXAS32ELEMENTGEDETECTOR_H
#define BIOXAS32ELEMENTGEDETECTOR_H

#include "beamline/AMXspress3XRFDetector.h"

/// Implementation of AMXRFDetector for the 32-element Germanium detector on BioXAS.
class BioXAS32ElementGeDetector : public AMXspress3XRFDetector
{
	Q_OBJECT

public:
	/// Default constructor.  Requires the name and description of the detector. It builds all the PV's and connects them accordingly.
	BioXAS32ElementGeDetector(const QString &name, const QString &description, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXAS32ElementGeDetector();
};

#endif // BIOXAS32ELEMENTGEDETECTOR_H
