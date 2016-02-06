#ifndef SXRMBBRUKERDETECTOR_H
#define SXRMBBRUKERDETECTOR_H

#include "beamline/AMXRFDetector.h"
#include "application/SXRMB/SXRMB.h"

/// Implementation of AMXRFDetector for the single element Bruker detector used on SXRMB.
class SXRMBBrukerDetector : public AMXRFDetector
{
	Q_OBJECT

public:
	/// Default constructor.  Requires the name and description of the detector.  It builds all the PVs required by the detector.
	SXRMBBrukerDetector(const QString &name, const QString &description, QObject *parent = 0);
	/// Destructor.
	virtual ~SXRMBBrukerDetector();

	/// Returns a string with a human readable text of what is important about this detector.
	virtual QString details() const;
	/// Returns the type of the detector
	virtual int type() { return SXRMB::BrukerDetector; }
	/// Returns the dead time of deadTimeControl_.
	virtual double deadTime() const;
	/// Returns the dead time of deadTimeControl_.
	virtual double deadTimeAt(int index) const;

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
	virtual bool supportsElapsedTime() const { return true; }

public slots:

	/// The read mode cannot be changed for Vortex detectors
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode);

	/// Vortex detectors do not support clearing
	virtual bool clear() { return false; }

protected slots:
	/// handles deadTimeControl PV connect signal
	void onDeadTimeControlConnected(bool);

protected:
	/// Control handling the bruker deadtime.
	AMReadOnlyPVControl *deadTimeControl_;
};

#endif // SXRMBBRUKERDETECTOR_H
