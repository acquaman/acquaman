#ifndef BIOXASMAININBOARD32ELEMENTGEDETECTOR_H
#define BIOXASMAININBOARD32ELEMENTGEDETECTOR_H

#include "beamline/BioXAS/BioXAS32ElementGeDetector.h"

class BioXASMainInboard32ElementGeDetector : public BioXAS32ElementGeDetector
{
    Q_OBJECT

public:
	/// Default constructor.  Requires the name and description of the detector, as well as the necessary pulse block that the Xspress3 electronics are connected to. It builds all the PV's and connects them accordingly.
	BioXASMainInboard32ElementGeDetector(const QString &name, const QString &description, AMPVControl *triggerControl, BioXASZebraPulseControl *pulseControl, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMainInboard32ElementGeDetector();

	/// Returns whether the element can be enabled. Elements are zero indexed.
	virtual bool canEnableElement(int index) const;
};

#endif // BIOXASMAININBOARD32ELEMENTGEDETECTOR_H
