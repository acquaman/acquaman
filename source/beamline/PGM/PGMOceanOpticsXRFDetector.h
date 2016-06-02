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

	/// Returns the read method.
	virtual AMDetectorDefinitions::ReadMethod readMethod() const { return AMDetectorDefinitions::RequestRead; }
	/// Returns the read mode.
	virtual AMDetectorDefinitions::ReadMode readMode() const { return AMDetectorDefinitions::SingleRead; }

	/// Returns true if the detector supports elapsed time.
	virtual bool supportsElapsedTime() const { return false; }

public slots:
	/// The read mode cannot be changed for Amptek detectors
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode);
};

#endif // PGMOCEANOPTICSXRFDETECTOR_H
