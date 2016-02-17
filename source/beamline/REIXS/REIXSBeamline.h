/*
Copyright 2010-2012 Mark Boots, David Chevrier, Darren Hunter and David Muir.

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

#ifndef REIXSBEAMLINE_H
#define REIXSBEAMLINE_H

#include "beamline/CLS/CLSBeamline.h"
#include "beamline/AMControlSet.h"
#include "acquaman/REIXS/REIXSXESMCPDetectorPre2013.h"	///< \todo Move this to beamline, not acquaman.
#include "beamline/REIXS/REIXSXESMCPDetector.h"
#include "dataman/REIXS/REIXSXESCalibration2.h"
#include "beamline/AMCompositeControl.h"
#include "beamline/CLS/CLSMDriveMotorControl.h"

#include "util/AMDeferredFunctionCall.h"
#include "beamline/CLS/CLSExclusiveStatesControl.h"

class AMDetector;
class CLSSIS3820Scaler;

class AMSamplePlatePre2013;

class AMAction;

class REIXSBrokenMonoControl;

/// The REIXSPhotonSource control is a container for the set of controls that make up the mono and EPU
class REIXSPhotonSource : public AMCompositeControl {
	Q_OBJECT
public:
	virtual ~REIXSPhotonSource();
	REIXSPhotonSource(QObject* parent = 0);

	REIXSBrokenMonoControl* energy() { return energy_; }
	AMControl* userEnergyOffset() {return userEnergyOffset_; }
	AMControl* directEnergy() { return directEnergy_; }
	AMControl* bypassEnergy() { return bypassEnergy_; }
	AMControl* monoSlit() { return monoSlit_; }
	AMControl* monoGratingTranslation() { return monoGratingTranslation_; }
	AMControl* monoGratingSelector() { return monoGratingSelector_; }
	AMControl* monoMirrorTranslation() { return monoMirrorTranslation_; }
	AMControl* monoMirrorSelector() { return monoMirrorSelector_; }
	AMControl* monoMirrorAngleStatus() { return monoMirrorAngleStatus_; }
	AMControl* monoGratingAngleStatus() { return monoGratingAngleStatus_; }
	AMControl* epuPolarization() { return epuPolarization_; }
	AMControl* epuPolarizationAngle() { return epuPolarizationAngle_; }
	AMControl* ringCurrent()  { return ringCurrent_; }
	AMControl* M5Pitch() { return M5Pitch_; }
	AMControl* M5Yaw() { return M5Yaw_; }




protected:
	AMControl* directEnergy_, *bypassEnergy_,*userEnergyOffset_, *monoSlit_, *monoGratingTranslation_, *monoGratingSelector_, *monoGratingAngleStatus_, *monoMirrorAngleStatus_, *monoMirrorTranslation_, *monoMirrorSelector_, *epuPolarization_, *epuPolarizationAngle_, *M5Pitch_, *M5Yaw_, *ringCurrent_;
	REIXSBrokenMonoControl* energy_;

};

/// REIXSValvesAndShutters is a container for the set of valve and shutter controls on the beamline
class REIXSValvesAndShutters : public AMCompositeControl {
	Q_OBJECT
public:
	virtual ~REIXSValvesAndShutters();
	REIXSValvesAndShutters(QObject* parent = 0);

	/// Safety shutter 1
	CLSExclusiveStatesControl* ssh1() { return ssh1_; }
	/// Photon shutter 2
	CLSExclusiveStatesControl* psh2() { return psh2_; }
	/// Photon shutter 4: Used for turning off the beam
	CLSExclusiveStatesControl* psh4() { return psh4_; }
	/// Used in sample changes to isolate the endstation
	CLSExclusiveStatesControl* endstationValve() { return endstationValve_; }


	/// \todo Variable apertures: set to 4x4

	/// Returns true if the beam (to the best of our knowledge) is on. Currently, requires ssh1, psh2, and psh4 to be open.
	bool isBeamOn() const { return beamIsOn_; }

signals:
	void beamOnChanged(bool isOn);

protected slots:
	/// Called when any of ssh1, psh2, and psh4 connect, disconnect, or change state. Reviews beamIsOn_ and emits beamOnChanged()
	void reviewIsBeamOn();

protected:
	CLSExclusiveStatesControl* ssh1_, *psh2_, *psh4_, *endstationValve_;
	bool beamIsOn_;

};

/// The REIXSHexapod control is just a container for the set of coupled controls which make up the hexapod:
/*!
- x(), y(), z(): Stage position in mm
- u(), v(), w(): Stage angle, in deg
- r(), s(), t(): Rotation point relative to stage origin, in mm
*/

class REIXSHexapod : public AMCompositeControl {
	Q_OBJECT
public:
	virtual ~REIXSHexapod();
	REIXSHexapod(QObject* parent = 0);

	AMControl* x() { return x_; }
	AMControl* y() { return y_; }
	AMControl* z() { return z_; }
	AMControl* u() { return u_; }
	AMControl* v() { return v_; }
	AMControl* w() { return w_; }
	AMControl* r() { return r_; }
	AMControl* s() { return s_; }
	AMControl* t() { return t_; }



protected:
	/// Controls, connected to the hexapod PVs
	AMPVwStatusControl *x_, *y_, *z_, *u_, *v_, *w_;
	AMPVControl *r_, *s_, *t_;

};

class AMListAction3;

/// The REIXSSpectrometer control is a high-level abstraction for controlling the spectrometer energy.  It's also a container for the set of (low-level, physical) controls which make up the spectrometer:
/*!
- angleDrive(): The position of the ball screw that lifts the spectrometer, in mm, up from the home (lowered) position.
- detectorTranslation(): The position of the ball screw that translates the detector along the chamber, in mm, away from the upstream home position
- detectorTiltDrive():  The position of the linear stage that tilts the detector, in mm, up from its vertically lowest home position
- hexapod(): The REIXSHexapod controls
*/
class REIXSSpectrometer : public AMCompositeControl {
	Q_OBJECT
public:
	virtual ~REIXSSpectrometer();
	REIXSSpectrometer(QObject* parent = 0);

	/// The spectrometer calibration object we are using
	const REIXSXESCalibration2* spectrometerCalibration() const { return &calibration_; }
	// temporary, for commissioning.
	REIXSXESCalibration2* spectrometerCalibration() { return &calibration_; }

	int gratingCount() const { return calibration_.gratingCount(); }

	int grating() const { return currentGrating_; }
	int specifiedGrating() const { return specifiedGrating_; }

	double focusOffset() const { return currentFocusOffset_; }
	double specifiedFocusOffset() const { return specifiedFocusOffset_; }

	double detectorTiltOffset() const { return currentDetectorTiltOffset_; }
	double specifiedDetectorTiltOffset() const { return specifiedDetectorTiltOffset_; }


	/// the energy position (in eV) based on the current position of the spectrometer.  This uses the current grating setting (not the specified grating) and current calibration to figure it out. It's the best estimate of what energy the actual detector position corresponds to.  (However, there's no guarantee that the detector is focussed or at the right tilt, unless inSpecifiedPosition() = true).
	virtual double value() const;
	double eV() const { return value(); }

	/// The last energy setpoint the spectrometer was told to go
	double setpoint() const { return specifiedEV_; }


	/// this indicates whether the spectrometer is "in position".  For the spectrometer, this means that the current grating matches the specified grating, the current offsets match the specified offsets, the angle is within tolerance for the desired energy, and the hexapod is in position for the current grating.
	virtual bool inPosition() const {
		return fabs(value()-setpoint()) < tolerance() &&
				currentGrating_ == specifiedGrating_ &&
				gratingInPosition() &&
				currentFocusOffset_ == specifiedFocusOffset_ &&
				currentDetectorTiltOffset_ == specifiedDetectorTiltOffset_;
	}

	/// Returns true if the hexapod position is correct for the current grating().
	bool gratingInPosition() const;

	/// this indicates whether a contrl is "within tolerance" of a given target (ie: the target specified is the same as the current value within the set tolerance)
	virtual bool withinTolerance(double target) const { return fabs(value()-target) < tolerance(); }


	bool shouldMeasure() const { return true; }
	bool shouldMove() const { return true; }
	bool shouldStop() const { return true; }
	// exception: the hexapod can't stop (Without wrecking its calibration and requiring a re-init.) But it can be told to go to new positions, even if the old move isn't done, so don't worry about stopping it.
	bool canStop() const { return spectrometerRotationDrive_->canStop() && detectorTranslation_->canStop() && detectorTiltDrive_->canStop(); }


	/// Indicates that a move (that was commanded via this object using move()) is in progress.  This will be accompanied by signals moveStarted(), moveFinished(), moveFailed(int reason).
	virtual bool moveInProgress() const { return moveAction_ != 0; }

	/// Returns the minimum and maximum value for the current grating
	virtual double minimumValue() const { return calibration_.gratingAt(specifiedGrating_).evRangeMin(); }
	virtual double maximumValue() const { return calibration_.gratingAt(specifiedGrating_).evRangeMax(); }

	/// Move to the given energy, using the specified grating, focusOffset, tiltOffset, and the current calibration. (This will cause spectrometer motion)
	virtual FailureExplanation move(double setpoint);

	/// Stop the spectrometer if it's currently moving
	virtual bool stop();


	AMControl* spectrometerRotationDrive() { return spectrometerRotationDrive_; }
	AMControl* detectorTranslation() { return detectorTranslation_; }
	AMControl* detectorTiltDrive() { return detectorTiltDrive_; }
	AMControl* endstationTranslation() { return endstationTranslation_; }  //DAVID ADDED
	AMControl* gratingMask() { return gratingMask_; } //DAVID ADDED 005
	/*
	removed motor from endstation in Dec. 2011:
	AMControl* detectorRotationDrive() { return detectorRotationDrive_; }
	*/
	REIXSHexapod* hexapod() { return hexapod_; }
	AMControl* tmMCPPreamp() { return tmMCPPreamp_; }
	AMControl* tmSOE() { return tmSOE_; }


public slots:
	/// Specify which stored calibration to use.  Use a \c databaseId of 0 or -1 to reset to the default calibration (ie: a default-constructed REIXSXESCalibration).  Returns true on success.
	bool loadSpectrometerCalibration(AMDatabase* db, int databaseId);

	void specifyFocusOffset(double focusOffsetMm);
	bool specifyGrating(int gratingIndex);
	void updateGrating();
	void specifyDetectorTiltOffset(double tiltOffsetDeg);

protected slots:
	void onConnected(bool isConnected);

protected:
	AMPVwStatusControl *spectrometerRotationDrive_, *detectorTranslation_, *detectorTiltDrive_, *endstationTranslation_, *gratingMask_;  //DAVID ADDED 001, 005
	REIXSHexapod* hexapod_;

	REIXSXESCalibration2 calibration_;

//	Temperature
	AMControl* tmMCPPreamp_, *tmSOE_;
//  Beam Currents


	/// Current grating is -1 if a grating hasn't been positioned yet
	int currentGrating_, specifiedGrating_;
	double currentFocusOffset_, specifiedFocusOffset_;
	double currentDetectorTiltOffset_, specifiedDetectorTiltOffset_;

	double specifiedEV_;

	/// It takes lots of steps to move the detector into position. This is the action we use to run a detector move. Valid if a move is in progress, and 0 otherwise.
	AMListAction3 *moveAction_;

	/// Holds the values for all the motors we need to move, to reach an energy position.
	AMControlInfoList moveSetpoint_;

	/// We don't want to calculate and emit valueChanged() as often as we get motor move updates... because these ones can update real fast. This is used to slow it down, and only emit valueChanged() about once every 200ms.
	AMDeferredFunctionCall reviewValueChangedFunction_;

signals:
	/// Emitted when the calibration object is changed (This might mean that the # of gratings or grating names might be different). Check with spectrometerCalibration().
	void calibrationChanged();

	/// Emitted when the current grating() changes.  This doesn't necessarily mean that the new grating is in position, but we will have started moving it into position.  The specifiedGrating() turns into the current grating() when a move() is issued.
	void gratingChanged(int);

	/// \todo: gratingInPositionChanged() for any hexapod moves...
	/// \todo: current defocus, current tilt and signals for them changing.

protected slots:

	/// Called whenever the lift and translation motors move, this schedules valueChanged() to be emitted, but not as fast as the motors update.
	void scheduleReviewValueChanged() { reviewValueChangedFunction_.runLater(200); }
	/// Called at a maximum rate of 0.1s, this examines the current position of the spectrometer and emits valueChanged().
	void reviewValueChanged() { emit valueChanged(value()); }


	/// Called when the moveAction_ changes state. Handle success, failure, or cancellation.
	void onMoveActionStateChanged(int newState, int previousState);
};

/// The REIXSSampleChamber control is a container for the motor controls that make up the sample manipulator and load lock.
class REIXSSampleChamber : public AMCompositeControl {
	Q_OBJECT
public:
	virtual ~REIXSSampleChamber();
	REIXSSampleChamber(QObject* parent = 0);

	AMControl* x() { return x_; }
	AMControl* y() { return y_; }
	AMControl* z() { return z_; }
	AMControl* r() { return r_; }

	AMControl* loadLockZ() { return loadLockZ_; }
	AMControl* loadLockR() { return loadLockR_; }



protected:
	CLSMDriveMotorControl* x_, *y_, *z_, *r_, *loadLockZ_, *loadLockR_;

};

/// This control provides a wrapper around the beamline energy PV control to correct some of its major deficiencies.
/*!
- For moves larger than repeatMoveThreshold(), it sends the same move command repeatMoveAttempts() times, pausing for repeatMoveSettlingTime() seconds after each (to wait for the running-average of the encoder signal to catch up).
- For moves with targets below lowEnergyThreshold() [100eV], it does the sub-100eV part of the move in lowEnergyStepSize() [1eV] increments, to avoid crashing the mirror and grating together when they get out of sync.

None of this would be necessary if the mono motors did not get out of sync with each other, and actually went to where they were instructed to go without losing many many steps. */

class REIXSBrokenMonoControl : public AMControl {
	Q_OBJECT
public:
	/// Construct with the AMPVwStatusControl to wrap \c control, the threshold to trigger multiple sub-moves, the number of sub-move attempts, the move settling time after each sub-move, the low energy threshold, and the low energy step size. We take ownership of the control.
	REIXSBrokenMonoControl(AMPVwStatusControl* underlyingControl, double repeatMoveThreshold = 1.05, int repeatMoveAttempts = 3, double repeatMoveSettlingTime = 0.3, double singleMoveSettlingTime = 0.05, double lowEnergyThreshold = 150, double lowEnergyStepSize = 1.0, double actualTolerance = 0.1, QObject* parent = 0);

	/// Destructor: deletes the underlying control.
	virtual ~REIXSBrokenMonoControl();

	/// Returns the feedback from the underlying control
	virtual double value() const { return control_->value(); }
	/// Returns the last setpoint requested by move().
	virtual double setpoint() const { return setpoint_; }

	/// Connected depends on whether the underlying control is connected.
	virtual bool isConnected() const { return control_->isConnected(); }

	virtual bool canMeasure() const { return control_->canMeasure(); }
	virtual bool canMove() const { return control_->canMove(); }
	virtual bool shouldMeasure() const { return control_->shouldMeasure(); }
	virtual bool shouldMove() const { return control_->shouldMove(); }
	virtual bool canStop() const { return control_->canStop(); }
	virtual bool shouldStop() const { return control_->shouldStop(); }

	virtual bool allowsMovesWhileMoving() const { return false; }

	/// We're moving if either: the control is moving (possibly externally), or one of our extended moves is in progress [even though the underlying control might be stopped temporarily]
	virtual bool isMoving() const { return control_->isMoving() || moveAction_; }
	/// One of OUR moves is in progress whenever our move action is valid.
	virtual bool moveInProgress() const { return moveAction_; }

	virtual double minimumValue() const { return control_->minimumValue(); }
	virtual double maximumValue() const { return control_->maximumValue(); }

	virtual int alarmSeverity() const { return control_->alarmSeverity(); }
	virtual int alarmStatus() const { return control_->alarmStatus(); }


	/// Returns the settling time for repeated (multi-step) moves. This much time is allowed before checking the feedback value for tolerance.
	double repeatMoveSettlingTime() const { return repeatMoveSettlingTime_; }
	/// Returns the settling time for single-step moves. This much time is allowed before checking the feedback value for tolerance.
	double singleMoveSettlingTime() const { return singleMoveSettlingTime_; }

public slots:

	/// Used to start a move to \c setpoint. Returns NoFailure if the move command was sent successfully. Our re-implementation creates and populates the moveAction_.
	virtual FailureExplanation move(double setpoint);

	/// Request to stop a move in progress.
	virtual bool stop();


	/// Set the settling time for repeated (multi-step) moves. This much time is allowed before checking the feedback value for tolerance. Set to 0 to disable.
	void setRepeatMoveSettlingTime(double seconds) { repeatMoveSettlingTime_ = seconds; }
	/// Set the settling time for single-step moves. This much time is allowed before checking the feedback value for tolerance. Set to 0 to disable.
	void setSingleMoveSettlingTime(double seconds) { singleMoveSettlingTime_ = seconds; }


protected slots:

	/// called when the underlying control emits movingChanged(). We should combine with our definition of isMoving().
	void onControlMovingChanged(bool isMoving);

	/// Called while one of our moves is in progress, and the move action fails or is cancelled.
	void onMoveActionFailed();
	/// Called while one of our moves is in progress, and the complete move action succeeded.
	void onMoveActionSucceeded();
	/// Called when monoMirrorAngle or monoGratingAngle reports an error, which stalls the energy application unless the move is stopped and restarted
	void onMonoAngleError(double error);

protected:
	double repeatMoveThreshold_, repeatMoveSettlingTime_, singleMoveSettlingTime_, lowEnergyThreshold_, lowEnergyStepSize_;
	int repeatMoveAttempts_;
	AMPVwStatusControl* control_;
	double setpoint_;

	bool stopInProgress_;

	/// Used to run the steps of a move. 0 if one of our special moves is not in progress.
	AMListAction3 *moveAction_;

	/// Used for change detection of isMoving()
	bool wasMoving_;

};


/// This class creates and provides access to the AMControl objects with the power to move the REIXS beamline and spectrometer
class REIXSBeamline : public CLSBeamline
{
	Q_OBJECT
public:
	/// Acccess the REIXSBeamline singleton instance through REIXSBeamline::bl()
	static REIXSBeamline* bl() {
		if(!instance_){
			instance_ = new REIXSBeamline();
			instance_->initializeBeamlineSupport();
		}
		return static_cast<REIXSBeamline*>(instance_);
	}

	/// Destructor
	virtual ~REIXSBeamline();

	// Accessing control elements:

	/// Access the upstream (mono/EPU) controls
	REIXSPhotonSource* photonSource() { return photonSource_; }
	/// Access the spectrometer controls:
	REIXSSpectrometer* spectrometer() { return spectrometer_; }
	/// Access the sample chamber and load-lock controls:
	REIXSSampleChamber* sampleChamber() { return sampleChamber_; }
	/// Access the live MCP detector object
	REIXSXESMCPDetector* mcpDetector() { return mcpDetector_; }
	/// Access the valves and shutters
	REIXSValvesAndShutters* valvesAndShutters() { return valvesAndShutters_; }
	/// Returns the current (active) sample plate, ie:the one that is currently loaded. When a user uses the UI to switch sample plates, we simple re-load this one from the database to become a different sample plate.
	AMSamplePlatePre2013* samplePlate() { return samplePlate_; }
	int currentSamplePlateId() const;
	/// Returns the id of the sample on the current plate that is in position.
	int currentSampleId();
	virtual AMControlSet* currentSamplePositioner() { return sampleManipulatorSet_; }


	// These Control Sets are logical groups of controls, that are commonly used by different Acquaman components

	/// The sampleX, sampleY, sampleZ, and sampleR (ie: theta) controls for moving the sample plate, used by the Sample Manipulator
	AMControlSet* sampleManipulatorSet() { return sampleManipulatorSet_; }
	/// All the controls for positioning the Spectrometer
	AMControlSet* spectrometerPositionSet() { return spectrometerPositionSet_; }
	/// All the controls we want to expose to users for available motions in REIXSControlMoveAction.
	AMControlSet* allControlsSet() { return 0; }
	/// All temperature monitors set
	AMControlSet* tmset() { return tmSet_; }
	/// Meter Set
	AMControlSet* meterSet() { return meterSet_;}

	virtual CLSSIS3820Scaler *scaler() const { return scaler_; }

	AMControl* I0Current() { return i0Current_; }
	AMControl* TEYCurrent() { return teyCurrent_; }


	/// Build a list of actions that opens/closes necessary shutters.
	AMAction3 *buildBeamStateChangeAction(bool beamOn) const;

signals:

public slots:


protected:
	/// Constructor. This is a singleton class, access it through REIXSBeamline::bl().
	REIXSBeamline();

	void setupExposedControls();
	void setupExposedDetectors();

protected:

	/// A group of controls making up the EPU and mono
	REIXSPhotonSource* photonSource_;
	/// A hierarchichal group of controls making up the spectrometer
	REIXSSpectrometer* spectrometer_;
	/// A hierarchichal group of controls making up the sample chamber
	REIXSSampleChamber* sampleChamber_;
	/// An object for controlling the MCP detector and downloading its image values
	REIXSXESMCPDetector* mcpDetector_;
	/// A group of valve and shutter controls
	REIXSValvesAndShutters* valvesAndShutters_;


	// These Control Sets are logical groups of controls, that are commonly used by different Acquaman components
	/// The sampleX, sampleY, sampleZ, and sampleR (ie: theta) controls for moving the sample plate, used by the Sample Manipulator
	AMControlSet* sampleManipulatorSet_;
	/// All the controls for positioning the Spectrometer (angleDrive, detectorTranslation, detectorTiltDrive, detectorRotationDrive, hexapod{X, Y, Z, U, V, W, R, S, T}
	AMControlSet* spectrometerPositionSet_;
	/// All tempertature monitors
	AMControlSet* tmSet_;
	/// Various meter monitors
	AMControlSet* meterSet_;

	/// This is the active sample plate object, ie:the one that is currently loaded. When a user uses the UI to switch sample plates, we simple re-load this one from the database to become a different sample plate.
	AMSamplePlatePre2013* samplePlate_;

	CLSSIS3820Scaler *scaler_;

	AMDetector* i0Detector_;
	AMDetector* teyDetector_;
	AMDetector* tfyDetector_;
	AMDetector* pfyDetector_;
	AMControl *i0Current_;
	AMControl *teyCurrent_;
};

#endif // REIXSBEAMLINE_H
