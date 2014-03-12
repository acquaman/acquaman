#ifndef IDEASKETEKDETECTOR_H
#define IDEASKETEKDETECTOR_H

#include "beamline/AMXRFDetector.h"

/// Implementation of the AMXRFDetector for the KETEK fluorescence detector for the IDEAS beamline.
class IDEASKETEKDetector : public AMXRFDetector
{
    Q_OBJECT

public:
    /// Constructor.  Requires the name and description of the detector.  It builds all the necessary controls.
    IDEASKETEKDetector(const QString &name, const QString &description, QObject *parent = 0);

    /// The KETEK doesn't explicitly require powering on
    virtual bool requiresPower() const { return false; }

    /// Cancelling is implemented for the KETEK detectors
    virtual bool canCancel() const { return true; }
    /// Clearing is not currently supported for the KETEK detectors
    virtual bool canClear() const { return false; }

    /// KETEK detectors are not currently capable of continuous acquisition
    virtual bool canContinuousAcquire() const { return false; }

    /// The KETEK can be configured to work with synchronized dwell time systems
    virtual bool supportsSynchronizedDwell() const { return true; }
    /// Returns the CLS Synchronized Dwell Time trigger PV string, which acts as the key for the synchronized dwell time lookup system
    virtual QString synchronizedDwellKey() const;

    /// The KETEK detectors share a triggering source  sometimes uses the synchronized dwell time object
    virtual bool sharesDetectorTriggerSource();
    /// Returns the synchronized dwell time trigger source if we're currently enabled, otherwise a null pointer
    virtual AMDetectorTriggerSource* detectorTriggerSource();
    /// Returns the synchronized dwell time dwell time source if we're currently enabled, otherwise a null pointer
    virtual AMDetectorDwellTimeSource* detectorDwellTimeSource();

    /// Returns RequestRead as the type
    virtual AMDetectorDefinitions::ReadMethod readMethod() const { return AMDetectorDefinitions::RequestRead; }
    /// Returns SingleRead as the type
    virtual AMDetectorDefinitions::ReadMode readMode() const { return AMDetectorDefinitions::SingleRead; }

    /// Returns false, because the KETEK detectors do not support continuous reads
    virtual bool lastContinuousReading(double *outputValues) const;

    /// The KETEK detectors support elapsed time.
    virtual bool supportsElapsedTime() const { return true; }

public slots:

        /// The read mode cannot be changed for Vortex detectors
        virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode);

        /// Vortex detectors do not support clearing
        virtual bool clear() { return false; }
};

#endif // IDEASKETEKDETECTOR_H
