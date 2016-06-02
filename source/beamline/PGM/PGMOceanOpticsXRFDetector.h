#ifndef PGMOCEANOPTICSXRFDETECTOR_H
#define PGMOCEANOPTICSXRFDETECTOR_H

#include "beamline/AMXRFDetector.h"
#include "application/PGM/PGM.h"

class PGMOceanOpticsXRFDetector : public AMXRFDetector
{
	Q_OBJECT

public:
	/// Constructor.
	PGMOceanOpticsXRFDetector(const QString &name, const QString &description, QObject *parent = 0);
	/// Destructor.
	virtual ~PGMOceanOpticsXRFDetector();

	/// Returns the type of PGM detector
	virtual int type() { return PGM::OceanOpticsDetector; }
	/// Returns a string with a human readable text of what is important about this detector.
	virtual QString details() const;

	/// Returns true if the detector explicitly requires powering on.
	virtual bool requiresPower() const { return false; }

	/// Returns true if the detector supports cancelling.
	virtual bool canCancel() const { return false; }
	/// Returns true if the detector supports clearing.
	virtual bool canClear() const { return false; }

	/// Returns true if the detector supports continuous acquisitions.
	virtual bool canContinuousAcquire() const { return false; }

	/// Returns true if the detector supports synchronized dwell time.
	virtual bool supportsSynchronizedDwell() const { return false; }
	/// Returns the CLS Synchronized Dwell Time trigger PV string, which acts as the key for the synchronized dwell time lookup system
	virtual QString synchronizedDwellKey() const { return QString(); }

//	/// Returns true if the detector shares a detector trigger source.
//	virtual bool sharesDetectorTriggerSource() const;
//	/// Returns the synchronized dwell time trigger source if we're currently enabled, otherwise a null pointer
//	virtual AMDetectorTriggerSource* detectorTriggerSource();
//	/// Returns the synchronized dwell time dwell time source if we're currently enabled, otherwise a null pointer
//	virtual AMDetectorDwellTimeSource* detectorDwellTimeSource();

	/// Returns the read method.
	virtual AMDetectorDefinitions::ReadMethod readMethod() const { return AMDetectorDefinitions::RequestRead; }
	/// Returns the read mode.
	virtual AMDetectorDefinitions::ReadMode readMode() const { return AMDetectorDefinitions::SingleRead; }

	/// Returns true if the detector supports elapsed time.
	virtual bool supportsElapsedTime() const { return false; }

public slots:
	/// The read mode cannot be changed for Amptek detectors
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode);

//	/// Clears the detector, returns true if successful.
//	virtual bool clear() { return false; }

protected slots:
//	/// Handle triggering with respect to the trigger source.
//	virtual void onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode readMode);
//	/// This function is called from the Cancelling (acquisition) state for detectors that support cancelling acquisitions. Once the detector has successfully cancelled the acquisition you must call setAcquisitionCancelled()
//	virtual bool cancelAcquisitionImplementation();
};

#endif // PGMOCEANOPTICSXRFDETECTOR_H
