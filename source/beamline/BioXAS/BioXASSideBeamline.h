#ifndef BIOXASSIDEBEAMLINE_H
#define BIOXASSIDEBEAMLINE_H

#include "beamline/AMBeamline.h"
#include "beamline/AMControlSet.h"
#include "beamline/CLS/CLSSynchronizedDwellTime.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/AMMotorGroup.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"


#include "util/AMErrorMonitor.h"
#include "util/AMBiHash.h"

#include "beamline/CLS/CLSBasicScalerChannelDetector.h"
#include "beamline/CLS/CLSBasicCompositeScalerChannelDetector.h"

class CLSMAXvMotor;

class BioXASSideBeamline : public AMBeamline
{
	Q_OBJECT

public:
	/// Returns the instance of the beamline that has been created.
	static BioXASSideBeamline* bioXAS()
	{
		if(!instance_)
			instance_ = new BioXASSideBeamline();

		return static_cast<BioXASSideBeamline*>(instance_);
	}

	/// Destructor.
	virtual ~BioXASSideBeamline();

    /// Returns the scaler.
    CLSSIS3820Scaler* scaler();

    CLSMAXvMotor* m1UpperSlit();

    virtual bool isConnected() const;

protected slots:
    void onScalerConnectedChanged(bool connectionState);
    void onM1UpperSlitConnectedChanged(bool connectionState);

protected:
	/// Sets up the synchronized dwell time.
	void setupSynchronizedDwellTime();
	/// Sets up the readings such as pressure, flow switches, temperature, etc.
	void setupDiagnostics();
	/// Sets up logical groupings of controls into sets.
	void setupControlSets();
	/// Sets up all the detectors.
	void setupDetectors();
	/// Sets up the sample stage motors.
	void setupSampleStage();
	/// Sets up mono settings.
	void setupMono();
	/// Sets up various beamline components.
	void setupComponents();
	/// Sets up the exposed actions.
	void setupExposedControls();
	/// Sets up the exposed detectors.
	void setupExposedDetectors();
	/// Sets up the motor group for the various sample stages.
	void setupMotorGroup();
	/// Sets up all of the detectors that need to be added to scans that aren't a part of typical detectors.  This may just be temporary, not sure.
	void setupControlsAsDetectors();

	/// Constructor. This is a singleton class; access it through BioXASSideBeamline::bioXAS().
    BioXASSideBeamline();

protected:
    CLSSIS3820Scaler *scaler_;
    CLSMAXvMotor *m1UpperSlit_;
    bool wasConnected_;
};

#endif // BIOXASSIDEBEAMLINE_H
