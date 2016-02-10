/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef IDEASKETEKDETECTOR_H
#define IDEASKETEKDETECTOR_H

#include "beamline/AMXRFDetector.h"
#include "application/IDEAS/IDEAS.h"

/// Implementation of the AMXRFDetector for the KETEK fluorescence detector for the IDEAS beamline.
class IDEASKETEKDetector : public AMXRFDetector
{
    Q_OBJECT

public:
    /// Constructor.  Requires the name and description of the detector.  It builds all the necessary controls.
    IDEASKETEKDetector(const QString &name, const QString &description, QObject *parent = 0);

    /// Destructor
    virtual ~IDEASKETEKDetector();

    /// Returns the type of the detector
    virtual int type() { return IDEAS::Ketek; }
    /// Returns a string with a human readable text of what is important about this detector.
    virtual QString details() const;

    /// The KETEK doesn't explicitly require powering on
    virtual bool requiresPower() const { return false; }

    /// Cancelling is implemented for the KETEK detectors
    virtual bool canCancel() const { return true; }
    /// Clearing is not currently supported for the KETEK detectors
    virtual bool canClear() const { return false; }

    /// KETEK detectors are not currently capable of continuous acquisition
    virtual bool canContinuousAcquire() const { return false; }

    /// The KETEK can be configured to work with synchronized dwell time systems
    virtual bool supportsSynchronizedDwell() const { return false; }
    /// Returns the CLS Synchronized Dwell Time trigger PV string, which acts as the key for the synchronized dwell time lookup system
    virtual QString synchronizedDwellKey() const;

    /// The KETEK detectors share a triggering source  sometimes uses the synchronized dwell time object
    virtual bool sharesDetectorTriggerSource() const;
    /// Returns the synchronized dwell time trigger source if we're currently enabled, otherwise a null pointer
    virtual AMDetectorTriggerSource* detectorTriggerSource();
    /// Returns the synchronized dwell time dwell time source if we're currently enabled, otherwise a null pointer
    virtual AMDetectorDwellTimeSource* detectorDwellTimeSource();

    /// Returns RequestRead as the type
    virtual AMDetectorDefinitions::ReadMethod readMethod() const { return AMDetectorDefinitions::RequestRead; }
    /// Returns SingleRead as the type
    virtual AMDetectorDefinitions::ReadMode readMode() const { return AMDetectorDefinitions::SingleRead; }

    /// The KETEK detectors support elapsed time.
    virtual bool supportsElapsedTime() const { return true; }

    /// Returns AMPVControl for peaking time
    double peakingTime() const { return peakingTimeControl_->value(); }
    /// Returns AMPVControl for preamp gain
    double preampGain() const  { return preampGainControl_->value(); }

    AMControl *triggerLevel() const { return ketekTriggerLevel_; }
    AMControl *baselineThreshold() const { return ketekBaselineThreshold_; }
    /// Returns the real time for the KETEK.
    AMDetector *dwellTime() const {return ketekRealTime_; }


signals:
    void peakingTimeChanged(double);

public slots:

    /// The read mode cannot be changed for Vortex detectors
    virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode);

    /// Vortex detectors do not support clearing
    virtual bool clear() { return false; }

    /// Sets the peaking time on the detector through peakingTimeControl
    void setPeakingTime(double time);
    /// Sets the peaking time on the detector through preampGainControl
    void setPreampGain(double value);


protected:

    AMReadOnlyPVControl *realTimeControl_;
    AMDetector *ketekRealTime_;

    AMPVControl *peakingTimeControl_;

    AMControl *ketekTriggerLevel_;
    AMControl *ketekBaselineThreshold_;
    AMControl *preampGainControl_;
};

#endif // IDEASKETEKDETECTOR_H
