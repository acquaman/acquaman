#ifndef BIOXASSIDE32ELEMENTGEDETECTOR_H
#define BIOXASSIDE32ELEMENTGEDETECTOR_H

#include "beamline/BioXAS/BioXAS32ElementGeDetector.h"

class BioXASSide32ElementGeDetector : public BioXAS32ElementGeDetector
{
    Q_OBJECT

public:
	/// Default constructor.  Requires the name and description of the detector, as well as the necessary pulse block that the Xspress3 electronics are connected to. It builds all the PV's and connects them accordingly.
	BioXASSide32ElementGeDetector(const QString &name, const QString &description, AMPVControl *triggerControl, BioXASZebraPulseControl *pulseControl, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSide32ElementGeDetector();
};

#endif // BIOXASSIDE32ELEMENTGEDETECTOR_H
