#ifndef VESPERSCCDBASICDETECTOREMULATOR_H
#define VESPERSCCDBASICDETECTOREMULATOR_H

#include "beamline/AMBasicControlDetectorEmulator.h"
#include "beamline/VESPERS/VESPERSCCDDetector.h"

/// This class only provides a means of having a dwell key set for the detector.
class VESPERSCCDBasicDetectorEmulator : public AMBasicControlDetectorEmulator
{
	Q_OBJECT

public:
	/// Constructor.  Provide everything the normal emulator requires but also the VESPERSCCD..
	VESPERSCCDBasicDetectorEmulator(VESPERSCCDDetector *ccd, const QString &name, const QString &description, AMControl *control, AMControl *statusControl, double statusAcquiringValue, double statusNotAcquiringValue, AMDetectorDefinitions::ReadMethod readMethod,  QObject *parent = 0);

	/// The cannot be configured in this manner
	virtual bool supportsSynchronizedDwell() const { return ccd_->supportsSynchronizedDwell(); }
	/// Returns empty string
	virtual QString synchronizedDwellKey() const { return ccd_->synchronizedDwellKey();}
	/// Returns the trigger source.
	virtual AMDetectorTriggerSource* detectorTriggerSource() { return ccd_->detectorTriggerSource(); }
	/// Returns the synchronized dwell time dwell time source if we're currently enabled, otherwise a null pointer
	virtual AMDetectorDwellTimeSource* detectorDwellTimeSource() { return ccd_->detectorDwellTimeSource(); }

protected:
	/// The pointer to the CCD.
	VESPERSCCDDetector *ccd_;
};

#endif // VESPERSCCDBASICDETECTOREMULATOR_H
