#ifndef BIOXAS32ELEMENTGEDETECTOR_H
#define BIOXAS32ELEMENTGEDETECTOR_H

#include "beamline/AMXspress3XRFDetector.h"
#include "beamline/AMBasicControlDetectorEmulator.h"
#include "beamline/AMDetectorSet.h"
#include "application/BioXAS/BioXAS.h"
#include "beamline/BioXAS/BioXASZebraPulseControl.h"

/// Implementation of AMXRFDetector for the 32-element Germanium detector on BioXAS.
class BioXAS32ElementGeDetector : public AMXspress3XRFDetector
{
	Q_OBJECT

public:
	/// Default constructor.  Requires the name and description of the detector, as well as the necessary pulse block that the Xspress3 electronics are connected to. It builds all the PV's and connects them accordingly.
	BioXAS32ElementGeDetector(const QString &name, const QString &description, AMPVControl *triggerControl, BioXASZebraPulseControl *pulseControl, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXAS32ElementGeDetector();

	/// Returns the type of the detector
	virtual int type() { return BioXAS::Ge13ElementDetector; }

public slots:
	/// Set the acquisition dwell time for triggered (RequestRead) detectors
	virtual bool setAcquisitionTime(double seconds);

protected:
	/// The pulse control associated with the 32 element.
	BioXASZebraPulseControl *pulseControl_;
};

#endif // BIOXAS32ELEMENTGEDETECTOR_H
