#ifndef BIOXAS32ELEMENTGEDETECTOR_H
#define BIOXAS32ELEMENTGEDETECTOR_H

#include "beamline/AMXRFDetector.h"

/// Implementation of AMXRFDetector for the 32-element Germanium detector on BioXAS.
class BioXAS32ElementGeDetector : public AMXRFDetector
{
	Q_OBJECT

public:
	/// Default constructor.  Requires the name and description of the detector. It builds all the PV's and connects them accordingly.
	BioXAS32ElementGeDetector(const QString &name, const QString &description, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXAS32ElementGeDetector();

	/// The Vortex doesn't explicitly require powering on
	virtual bool requiresPower() const { return false; }

	/// Cancelling is implemented for the Vortex detectors
	virtual bool canCancel() const { return true; }
	/// Clearing is not currently supported for the Vortex detectors
	virtual bool canClear() const { return false; }

	/// Vortex detectors are not currently capable of continuous acquisition
	virtual bool canContinuousAcquire() const { return false; }

	/// The Vortex can be configured to work with synchronized dwell time systems
	virtual bool supportsSynchronizedDwell() const { return false; }
	/// Returns the CLS Synchronized Dwell Time trigger PV string, which acts as the key for the synchronized dwell time lookup system
	virtual QString synchronizedDwellKey() const;

	/// Returns RequestRead as the type
	virtual AMDetectorDefinitions::ReadMethod readMethod() const { return AMDetectorDefinitions::RequestRead; }
	/// Returns SingleRead as the type
	virtual AMDetectorDefinitions::ReadMode readMode() const { return AMDetectorDefinitions::SingleRead; }

	/// Returns false, because the Vortex detectors do not support continuous reads
	virtual bool lastContinuousReading(double *outputValues) const;
	/// The vortex detectors support elapsed time.
	virtual bool supportsElapsedTime() const { return false; }

public slots:

	/// The read mode cannot be changed for Amptek detectors
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode);

	/// Vortex detectors do not support clearing
	virtual bool clear() { return false; }

protected slots:
	/// Handles updating the acquisition state of the detector.
	void updateAcquisitionState();
};

#endif // BIOXAS32ELEMENTGEDETECTOR_H
