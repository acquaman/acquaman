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
#include "beamline/CLS/CLSKeithley428.h"
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

    /// Returns the m1 upper slit blade motor control.
    CLSMAXvMotor* m1UpperSlit();

    /// Returns an instance of the keithley428 amplifier.
    CLSKeithley428* keithley();

    virtual bool isConnected() const;

    CLSBasicScalerChannelDetector* testDetector();

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

    CLSBasicScalerChannelDetector *testDetector_;
    CLSKeithley428 *keithley_;

    // Shutters
    CLSBiStateControl *psh1_;
    CLSBiStateControl *psh2_;
    CLSBiStateControl *ssh1_;

//    // Pressure controls
    AMControl *ccg1_;
    AMControl *ccg2_;
//    AMControl *ccg3_;

    // vacuum value controls
    CLSBiStateControl *vvr1_;
//    CLSBiStateControl *vvr2_;
    CLSBiStateControl *vvr3_;
    CLSBiStateControl *vvr4_;
    CLSBiStateControl *vvr5_;

    // Ion pump controls
    AMControl *iop1_;
    AMControl *iop2_;
    AMControl *iop3_;
    AMControl *iop4_;
    AMControl *iop5_;

    // Flow transducer controls
    AMControl *flt1_;
    AMControl *flt2_;
    AMControl *flt3_;
    AMControl *flt4_;
    AMControl *flt5_;
    AMControl *flt6_;
    AMControl *flt7_;
    AMControl *flt8_;
    AMControl *flt9_;
    AMControl *flt10_;
    AMControl *flt11_;
    AMControl *flt12_;
    AMControl *flt13_;
    AMControl *flt14_;
    AMControl *flt15_;
    AMControl *flt16_;
    AMControl *flt17_;
    AMControl *flt18_;
    AMControl *flt19_;
    AMControl *flt20_;

    // Flow switch controls
    AMControl *swf1_;
    AMControl *swf2_;
    AMControl *swf3_;
    AMControl *swf4_;

    // Temperature monitor controls
    AMControl *tm1_;
    AMControl *tm2_;
    AMControl *tm3_;
    AMControl *tm4_;
    AMControl *tm5_;


};

#endif // BIOXASSIDEBEAMLINE_H
