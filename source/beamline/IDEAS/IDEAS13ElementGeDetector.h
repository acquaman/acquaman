#ifndef IDEAS13ELEMENTGEDETECTOR_H
#define IDEAS13ELEMENTGEDETECTOR_H

#include "beamline/AMXRFDetector.h"
#include "application/IDEAS/IDEAS.h"

/// Implementation of AMXRFDetector for the 13 element germanium detector on IDEAS.
class IDEAS13ElementGeDetector : public AMXRFDetector
{
	Q_OBJECT

public:
	/// Default constructor.  Requires the name and description of the detector.  It builds all necessary controls.
	IDEAS13ElementGeDetector(const QString &name, const QString &description, QObject *parent = 0);
	/// Destructor.
	~IDEAS13ElementGeDetector(){}

	/// Returns the type of the detector
	virtual int type() { return IDEAS::Ge13ElementDetector; }

	/// The germanium detector doesn't explicitly require powering on
	virtual bool requiresPower() const { return false; }

	/// Cancelling is implemented for the germanium detectors
	virtual bool canCancel() const { return true; }
	/// Clearing is not currently supported for the germanium detectors
	virtual bool canClear() const { return false; }

	/// Germanium detectors are not currently capable of continuous acquisition
	virtual bool canContinuousAcquire() const { return false; }

	/// The germanium can be configured to work with synchronized dwell time systems
	virtual bool supportsSynchronizedDwell() const { return false; }
	/// Returns the CLS Synchronized Dwell Time trigger PV string, which acts as the key for the synchronized dwell time lookup system
	virtual QString synchronizedDwellKey() const;

	/// The Vortex detectors share a triggering source  sometimes uses the synchronized dwell time object
	virtual bool sharesDetectorTriggerSource() const;
	/// Returns the synchronized dwell time trigger source if we're currently enabled, otherwise a null pointer
	virtual AMDetectorTriggerSource* detectorTriggerSource();
	/// Returns the synchronized dwell time dwell time source if we're currently enabled, otherwise a null pointer
	virtual AMDetectorDwellTimeSource* detectorDwellTimeSource();

	/// Returns RequestRead as the type
	virtual AMDetectorDefinitions::ReadMethod readMethod() const { return AMDetectorDefinitions::RequestRead; }
	/// Returns SingleRead as the type
	virtual AMDetectorDefinitions::ReadMode readMode() const { return AMDetectorDefinitions::SingleRead; }

	/// Returns false, because the Vortex detectors do not support continuous reads
	virtual bool lastContinuousReading(double *outputValues) const;
	/// The vortex detectors support elapsed time.
	virtual bool supportsElapsedTime() const { return true; }

	/// Returns the real time for the detector.
	AMDetector *dwellTime() const {return ge13ElementRealTime_; }

signals:


public slots:

	/// The read mode cannot be changed for Amptek detectors
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode);

	/// Vortex detectors do not support clearing
	virtual bool clear() { return false; }


protected:
	AMReadOnlyPVControl *ge13ElementRealTimeControl_;
	AMDetector *ge13ElementRealTime_;

    // preamp gain controls for detectors 1-12
    AMControl *preampGainControl1_, *preampGainControl2_, *preampGainControl3_, *preampGainControl4_,
              *preampGainControl5_, *preampGainControl6_, *preampGainControl7_, *preampGainControl8_,
              *preampGainControl9_, *preampGainControl10_,*preampGainControl11_,*preampGainControl12_;

    // energy peaking time
    AMPVControl *peakingTimeControl_;



};

#endif // IDEAS13ELEMENTGEDETECTOR_H
