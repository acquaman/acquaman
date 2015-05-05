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


#ifndef AM_SGMBEAMLINE_H
#define AM_SGMBEAMLINE_H

#include "beamline/CLS/CLSBeamline.h"
#include "dataman/SGM/SGMBeamlineInfo.h"

#include "beamline/AMDetectorGroup.h"
#include "beamline/AMDetectorSet.h"
#include "beamline/AMControlSet.h"
#include "util/AMBiHash.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/AMControlSetSampleManipulator.h"
#include "beamline/CLS/CLSSynchronizedDwellTime.h"

#define SGMBEAMLINE_PV_NAME_LOOKUPS_FAILED 312001

class AMSamplePlatePre2013;
class SGMMAXvMotor;
class AMMotorGroup;

class CLSAmptekSDD123DetectorNew;
class CLSPGTDetectorV2;
class CLSQE65000Detector;
class CLSAdvancedScalerChannelDetector;
class AMBasicControlDetectorEmulator;

class SGMBeamline : public CLSBeamline
{
Q_OBJECT
public:

	static SGMBeamline* sgm();		// singleton-class accessor
	virtual ~SGMBeamline();

	bool isConnected() const;
	bool isReady() const;
	bool isBeamlineScanning() const;
	bool isVisibleLightOn() const;

	/// Returns whether or not there's a valid X/Y for focus moving
	bool validInFocusPair() const;
	/// Returns whether or not we're currently moving in focus (validInFocusPair must return true for this to be true)
	bool movingInFocus() const;

	QStringList unconnectedCriticals() const;

	QString beamlineWarnings();

	QString pvName(const QString &amName) const { return amNames2pvNames_.valueF(amName);}
	QString amName(const QString &pvName) const { return amNames2pvNames_.valueR(pvName);}

	AMControl* energy() const { return energy_;}
	AMControl* energySpacingParam() const { return energySpacingParam_;}
	AMControl* energyC1Param() const { return energyC1Param_;}
	AMControl* energyC2Param() const { return energyC2Param_;}
	AMControl* energySParam() const { return energySParam_;}
	AMControl* energyThetaParam() const { return energyThetaParam_;}
	AMControl* exitSlitGap() const { return exitSlitGap_;}
	AMControl* entranceSlitGap() const { return entranceSlitGap_;}
	AMControl* mono() const { return energy_->childControlAt(0);}
	AMControl* undulator() const { return energy_->childControlAt(1);}
	AMControl* exitSlit() const { return energy_->childControlAt(2);}
	AMControl* m4() const { return m4_;}
	AMControl* grating() const { return grating_;}
	AMControl* harmonic() const { return harmonic_;}
	AMControl* undulatorStep() const { return undulatorStep_;}
	AMControl* undulatorRelativeStepStorage() const { return undulatorRelativeStepStorage_;}
	AMControl* undulatorVelocity() const { return undulatorVelocity_;}
	AMControl* undulatorFastTracking() const { return undulatorFastTracking_;}
	AMControl* undulatorFastTrackingTrigger() const { return undulatorFastTrackingTrigger_;}
	AMControl* undulatorTracking() const { return undulatorTracking_;}
	AMControl* undulatorForcedOpen() const { return undulatorForcedOpen_;}
	AMControl* monoTracking() const { return monoTracking_;}
	AMControl* exitSlitTracking() const { return exitSlitTracking_;}
	SGMMAXvMotor* ssaManipulatorX() const { return ssaManipulatorX_;}
	SGMMAXvMotor* ssaManipulatorY() const { return ssaManipulatorY_;}
	SGMMAXvMotor* ssaManipulatorZ() const { return ssaManipulatorZ_;}
	SGMMAXvMotor* ssaManipulatorRot() const { return ssaManipulatorRot_;}
	AMControl* beamlineScanning() const { return beamlineScanning_;}
	AMControl* beamlineReady() const { return beamlineReady_;}
	AMControl* nextDwellTimeTrigger() const { return nextDwellTimeTrigger_;}
	AMControl* nextDwellTimeConfirmed() const { return nextDwellTimeConfirmed_;}
	AMControl* energyMovingStatus() const { return energyMovingStatus_;}
	AMControl* fastShutterVoltage() const { return fastShutterVoltage_;}
	AMControl* gratingVelocity() const { return gratingVelocity_;}
	AMControl* gratingBaseVelocity() const { return gratingBaseVelocity_;}
	AMControl* gratingAcceleration() const { return gratingAcceleration_;}
	AMControl* ea1CloseVacuum1() const { return ea1CloseVacuum1_;}
	AMControl* ea1CloseVacuum2() const { return ea1CloseVacuum2_;}
	AMControl* ea2CloseVacuum() const { return ea2CloseVacuum_;}
	AMControl* beamOn() const { return beamOn_;}
	AMControl* visibleLightToggle() const { return visibleLightToggle_;}
	AMControl* visibleLightStatus() const { return visibleLightStatus_;}
	AMControl* activeEndstation() const { return activeEndstation_;}
	AMControl* scalerIntegrationTime() const { return scalerIntegrationTime_;}
	AMControl* ssaIllumination() const { return ssaIllumination_;}
	AMControl* tfyHVToggle() const { return tfyHVToggle_;}
	/// Returns the mirror selection feedback control (C or Si stripe)
	AMControl* mirrorStripeSelection() const { return mirrorStripeSelection_;}
	/// Returns the mirror selection control for Carbon
	AMControl* mirrorStripeSelectCarbon() const { return mirrorStripeSelectCarbon_;}
	/// Returns the mirror selection control for Silicon
	AMControl* mirrorStripeSelectSilicon() const { return mirrorStripeSelectSilicon_;}
	/// Returns the undulator offset control (for detuning)
	AMControl* undulatorOffset() const { return undulatorOffset_;}
	/// Returns the master dwell time for the synchronized dwell time application
	AMControl* masterDwell() const { return masterDwell_;}
	/// Returns the relative step for the undulator
	AMControl* undulatorRelativeStep() const { return undulatorRelativeStep_; }

	AMMotorGroup *motorGroup() const { return motorGroup_;}

	/// Returns the vertical upstream step for M2
	AMControl* m2VerticalUpstreamStep() const { return m2VerticalUpstreamStep_; }
	/// Returns the vertical downstream step for M2
	AMControl* m2VerticalDownstreamStep() const { return m2VerticalDownstreamStep_; }
	/// Returns the horizontal upstream step for M2
	AMControl* m2HorizontalUpstreamStep() const { return m2HorizontalUpstreamStep_; }
	/// Returns the horizontal downstream step for M2
	AMControl* m2HorizontalDownstreamStep() const { return m2HorizontalDownstreamStep_; }
	/// Returns the rotational step for M2
	AMControl* m2RotationalStep() const { return m2RotationalStep_; }
	/// Returns the vertical upstream step for M3
	AMControl* m3VerticalUpstreamStep() const { return m3VerticalUpstreamStep_; }
	/// Returns the vertical downstream step for M3
	AMControl* m3VerticalDownstreamStep() const { return m3VerticalDownstreamStep_; }
	/// Returns the horizontal upstream step for M3
	AMControl* m3HorizontalUpstreamStep() const { return m3HorizontalUpstreamStep_; }
	/// Returns the horizontal downstream step for M3
	AMControl* m3HorizontalDownstreamStep() const { return m3HorizontalDownstreamStep_; }
	/// Returns the rotational step for M3
	AMControl* m3RotationalStep() const { return m3RotationalStep_; }

	/// Returns the vertical upstream encoder for M2
	AMControl* m2VerticalUpstreamEncoder() const { return m2VerticalUpstreamEncoder_; }
	/// Returns the vertical downstream encoder for M2
	AMControl* m2VerticalDownstreamEncoder() const { return m2VerticalDownstreamEncoder_; }
	/// Returns the horizontal upstream encoder for M2
	AMControl* m2HorizontalUpstreamEncoder() const { return m2HorizontalUpstreamEncoder_; }
	/// Returns the horizontal downstream encoder for M2
	AMControl* m2HorizontalDownstreamEncoder() const { return m2HorizontalDownstreamEncoder_; }
	/// Returns the rotational encoder for M2
	AMControl* m2RotationalEncoder() const { return m2RotationalEncoder_; }
	/// Returns the vertical upstream encoder for M3
	AMControl* m3VerticalUpstreamEncoder() const { return m3VerticalUpstreamEncoder_; }
	/// Returns the vertical downstream encoder for M3
	AMControl* m3VerticalDownstreamEncoder() const { return m3VerticalDownstreamEncoder_; }
	/// Returns the horizontal upstream encoder for M3
	AMControl* m3HorizontalUpstreamEncoder() const { return m3HorizontalUpstreamEncoder_; }
	/// Returns the horizontal downstream encoder for M3
	AMControl* m3HorizontalDownstreamEncoder() const { return m3HorizontalDownstreamEncoder_; }
	/// Returns the rotational encoder for M3
	AMControl* m3RotationalEncoder() const { return m3RotationalEncoder_; }

	/// Returns the control for the valve in front of the bypass section
	AMControl* frontBypassValve() const { return frontBypassValve_; }
	/// Returns the control for the valve behind the bypass section
	AMControl* backBypassValve() const { return backBypassValve_; }

	virtual AMSynchronizedDwellTime* synchronizedDwellTime() const { return synchronizedDwellTime_;}

	/// Returns the validity of an action (see AMBeamline::ActionValidity). Currently the SGM responds that old XAS and Fast scans are AMBeamline::ActionNeverValid.
	virtual AMAction3::ActionValidity validateAction(AMAction3 *action);
	/// Returns messages for invalid actions (old scan configurations) and "Action is Currently Valid" for all other actions.
	virtual QString validateActionMessage(AMAction3 *action);

	SGMBeamlineInfo::sgmGrating currentGrating() const;
	QString currentEndstation() const;

	/// Returns a pointer to the scaler IF the scaler IS connected
	virtual CLSSIS3820Scaler* scaler() const;
	/// Returns a pointer to the scaler EVEN IF the scaler ISN'T yet connected
	CLSSIS3820Scaler* rawScaler();

	AMControlSetSampleManipulator* sampleManipulator() const { return sampleManipulator_; }
	virtual AMControlSet* currentSamplePositioner() { return ssaManipulatorSet(); }
	virtual QList<AMControlInfoList> currentFiducializations() { return ssaFiducializations(); }

	AMSamplePlatePre2013* currentSamplePlate() const { return currentSamplePlate_; }
	virtual int currentSamplePlateId() const;
	int currentSampleId();
	QString currentSampleDescription();

	AMControlSet* fluxResolutionSet() const { return fluxResolutionSet_;}
	AMControlSet* trackingSet() const { return trackingSet_;}
	AMControlSet* ssaManipulatorSet() const { return ssaManipulatorSet_; }
	QList<AMControlInfoList> ssaFiducializations() const { return ssaFiducializations_; }

	AMDetector* newAmptekSDD1() const;
	AMDetector* newAmptekSDD2() const;
	AMDetector* newAmptekSDD3() const;
	AMDetector* newAmptekSDD4() const;
	AMDetector* newAmptekSDD5() const;
	AMDetector* newPGTDetector() const;
	AMDetector* newQE65000Detector() const;
	AMDetector* newTEYDetector() const;
	AMDetector* newTFYDetector() const;
	AMDetector* newI0Detector() const;
	AMDetector* newPDDetector() const;
	AMDetector* newFilteredPD1Detector() const;
	AMDetector* newFilteredPD2Detector() const;
	AMDetector* newFilteredPD3Detector() const;
	AMDetector* newFilteredPD4Detector() const;
	AMDetector* newFilteredPD5Detector() const;
	AMDetector* newEncoderUpDetector() const;
	AMDetector* newEncoderDownDetector() const;
	AMDetector* energyFeedbackDetector() const;
	AMDetector* gratingEncoderDetector() const;

	AMDetector* dwellTimeDetector() const;

	AMDetectorGroup* allDetectorGroup() const { return allDetectorGroup_;}
	AMDetectorGroup* XASDetectorGroup() const { return XASDetectorGroup_;}
	AMDetectorGroup* FastDetectorGroup() const { return FastDetectorGroup_;}

	const AMControlSet* criticalControlsSet() const { return criticalControlsSet_; }
	const AMDetectorSet* criticalDetectorSet() const { return criticalDetectorSet_; }
	AMDetectorSet* criticalDetectorSet(const QObject *privilegedRequester);

	bool detectorConnectedByName(QString name);

	AMAction3* createBeamOnActions3();
	/// Turn off beam on SGM.
	AMAction3* createTurnOffBeamActions();
	AMAction3* createStopMotorsActions3();

	AMAction3* createGoToTransferPositionActions3();
	AMAction3* createGoToMeasurementPositionActions3();

	/// A list of actions that will restore the defaults for grating velocity/acceleration, scaler settings, and tracking settings to nominal values
	AMAction3* createRestorePreFastScanDefaultActions();

public slots:
	void setCurrentSamplePlate(AMSamplePlatePre2013 *newSamplePlate);

	void visibleLightOn();
	void visibleLightOff();

	void closeVacuum();

	void setCurrentEndstation(SGMBeamlineInfo::sgmEndstation endstation);
	void setCurrentMirrorStripe(SGMBeamlineInfo::sgmMirrorStripe mirrorStripe);

	/// Call to set the current SSA Manipulator X/Y values to an in-focus pair
	void setInFocus();
	/// Call to set the focus tracking on Y to follow X movements
	void setMoveInFocus(bool moveInFocus);

signals:
	void beamlineScanningChanged(bool scanning);
	void controlSetConnectionsChanged();
	void criticalControlsConnectionsChanged();
	void criticalConnectionsChanged();
	void beamlineReadyChanged();

	void beamlineConnected(bool isConnected);
	void detectorAvailabilityChanged(AMDetector *detector, bool isAvailable);

	void visibleLightStatusChanged(const QString& status);
	void beamlineWarningsChanged(const QString& warnings);
	void currentSamplePlateChanged(AMSamplePlatePre2013 *newSamplePlate);

	void currentEndstationChanged(SGMBeamlineInfo::sgmEndstation);
	void currentMirrorStripeChanged(SGMBeamlineInfo::sgmMirrorStripe);

	void beamlineInitialized();

	/// Emitted when the flag for Y tracking of X position
	void moveInFocusChanged(bool moveInFocus);

protected slots:
	void onBeamlineScanningValueChanged(double value);
	void onControlSetConnected(bool csConnected);
	void onCriticalControlsConnectedChanged(bool isConnected, AMControl *controll);
	void onCriticalsConnectedChanged();
	void onEnergyValueChanged();

	void onCriticalDetectorSetConnectedChanged(bool connected);
	void onAllDetectorGroupDetectorBecameConnected(AMDetector *detector);
	void onAllDetectorGroupDetectorBecameUnconnected(AMDetector *detector);

	void onActiveEndstationChanged(double value);
	void onMirrorStripeChanged(double value);

	void recomputeWarnings();

	void onVisibleLightChanged(double value);

	void computeBeamlineInitialized();

	void onAllDetectorsGroupAllDetectorResponded();

	/// Handles moving to the new Y focus position when X focus tracking is on
	void moveToYFocusPosition(double xSetpoint);

protected:
	void setupControls();
	void setupNameToPVLookup();

	/// Sets up the exposed controls for the SGM beamline (accessible through AMControlMoveAction)
	void setupExposedControls();

	/// Sets up the exposed detectors for the SGM beamline (accessible through the AMScanConfiguration/Controller interface)
	void setupExposedDetectors();

	void reviewConnected();

protected:
	// Singleton implementation:
	SGMBeamline();					// protected constructor... only access through Beamline::bl()

	SGMBeamlineInfo *infoObject_;

	// Parts of this beamline:
	///////////////////////////////

	AMControl *energy_;
	AMControl *energySpacingParam_;
	AMControl *energyC1Param_;
	AMControl *energyC2Param_;
	AMControl *energySParam_;
	AMControl *energyThetaParam_;
	AMControl *exitSlitGap_;
	AMControl *entranceSlitGap_;
	AMControl *m4_;
	AMControl *grating_;
	AMControl *harmonic_;
	AMControl *undulatorStep_;
	AMControl *undulatorRelativeStepStorage_;
	AMControl *undulatorVelocity_;
	AMControl *undulatorFastTracking_;
	AMControl *undulatorFastTrackingTrigger_;
	AMControl *undulatorTracking_;
	AMControl *undulatorForcedOpen_;
	AMControl *monoTracking_;
	AMControl *exitSlitTracking_;
	AMControl *tfyHV_;
	AMControl *tfyHVToggle_;
	CLSSynchronizedDwellTime *synchronizedDwellTime_;
	AMControl *pgtHV_;
	SGMMAXvMotor *ssaManipulatorX_;
	SGMMAXvMotor *ssaManipulatorY_;
	SGMMAXvMotor *ssaManipulatorZ_;
	SGMMAXvMotor *ssaManipulatorRot_;
	AMControl *beamlineScanning_;
	AMControl *beamlineReady_;
	AMControl *nextDwellTimeTrigger_;
	AMControl *nextDwellTimeConfirmed_;
	AMControl *energyMovingStatus_;
	AMControl *fastShutterVoltage_;
	AMControl *gratingVelocity_;
	AMControl *gratingBaseVelocity_;
	AMControl *gratingAcceleration_;
	AMControl *ea1CloseVacuum1_;
	AMControl *ea1CloseVacuum2_;
	AMControl *ea2CloseVacuum_;
	AMControl *beamOn_;
	AMControl *visibleLightToggle_;
	AMControl *visibleLightStatus_;
	AMControl *activeEndstation_;
	AMControl *scalerIntegrationTime_;
	AMControl *ssaIllumination_;
	/// Control for feedback on the mirror stripe (C or Si)
	AMControl *mirrorStripeSelection_;
	/// Control for sending mirror stripe to Carbon
	AMControl *mirrorStripeSelectCarbon_;
	/// Control for sending mirror stripe to Silicon
	AMControl *mirrorStripeSelectSilicon_;
	/// Control for detuning the undulator
	AMControl *undulatorOffset_;
	/// Control for the synchronized dwell time master dwell value
	AMControl *masterDwell_;
	/// Control for the relative step setpoint on the undulator gap motor
	AMControl *undulatorRelativeStep_;

	AMMotorGroup *motorGroup_;

	AMControl *m2VerticalUpstreamStep_;
	AMControl *m2VerticalDownstreamStep_;
	AMControl *m2HorizontalUpstreamStep_;
	AMControl *m2HorizontalDownstreamStep_;
	AMControl *m2RotationalStep_;
	AMControl *m3VerticalUpstreamStep_;
	AMControl *m3VerticalDownstreamStep_;
	AMControl *m3HorizontalUpstreamStep_;
	AMControl *m3HorizontalDownstreamStep_;
	AMControl *m3RotationalStep_;

	AMControl *m2VerticalUpstreamEncoder_;
	AMControl *m2VerticalDownstreamEncoder_;
	AMControl *m2HorizontalUpstreamEncoder_;
	AMControl *m2HorizontalDownstreamEncoder_;
	AMControl *m2RotationalEncoder_;
	AMControl *m3VerticalUpstreamEncoder_;
	AMControl *m3VerticalDownstreamEncoder_;
	AMControl *m3HorizontalUpstreamEncoder_;
	AMControl *m3HorizontalDownstreamEncoder_;
	AMControl *m3RotationalEncoder_;

	// Controls for the new valves on the bypass line
	AMControl *frontBypassValve_;
	AMControl *backBypassValve_;

	// Shutter controls (readonly), sorry for the names but it was either the PV name or
	// shutter_, anotherShutter_, yetAnotherShutter_ etc.
	AMControl *vvr1611_3_I10_01Shutter_;
	AMControl *vvr1611_3_I10_02Shutter_;
	AMControl *vvr1611_3_I10_03Shutter_;
	AMControl *vvr1611_3_I10_04Shutter_;
	AMControl *psh1611_3_I10_01Shutter_;
	AMControl *vvr1611_4_I10_01Shutter_;
	AMControl *vvr1611_4_I10_02Shutter_;
	AMControl *vvr1611_4_I10_03Shutter_;
	AMControl *vvr1611_4_I10_04Shutter_;
	AMControl *vvr1611_4_I10_05Shutter_;
	AMControl *vvr1611_4_I10_06Shutter_;
	AMControl *vvr1611_4_I10_07Shutter_;
	AMControl *vvr1611_4_I10_08Shutter_;
	AMControl *psh1411_I00_01Shutter_;
	AMControl *vvr1411_I00_01Shutter_;
	AMControl *vvf1411_I00_01Shutter_;
	AMControl *psh1411_I00_02Shutter_;
	AMControl *ssh1411_I00_01Shutter_;
	AMControl *vvr1611_3_I00_01Shutter_;
	AMControl *vvr1611_3_I00_02Shutter_;

	CLSAmptekSDD123DetectorNew *newAmptekSDD1_;
	CLSAmptekSDD123DetectorNew *newAmptekSDD2_;
	CLSAmptekSDD123DetectorNew *newAmptekSDD3_;
	CLSAmptekSDD123DetectorNew *newAmptekSDD4_;
	CLSAmptekSDD123DetectorNew *newAmptekSDD5_;
	CLSPGTDetectorV2 *newPGTDetector_;
	CLSQE65000Detector *newQE65000Detector_;
	CLSAdvancedScalerChannelDetector *newTEYDetector_;
	CLSAdvancedScalerChannelDetector *newTFYDetector_;
	CLSAdvancedScalerChannelDetector *newI0Detector_;
	CLSAdvancedScalerChannelDetector *newPDDetector_;
	CLSAdvancedScalerChannelDetector *newFilteredPD1Detector_;
	CLSAdvancedScalerChannelDetector *newFilteredPD2Detector_;
	CLSAdvancedScalerChannelDetector *newFilteredPD3Detector_;
	CLSAdvancedScalerChannelDetector *newFilteredPD4Detector_;
	CLSAdvancedScalerChannelDetector *newFilteredPD5Detector_;
	CLSAdvancedScalerChannelDetector *newEncoderUpDetector_;
	CLSAdvancedScalerChannelDetector *newEncoderDownDetector_;
	AMBasicControlDetectorEmulator *energyFeedbackDetector_;
	AMBasicControlDetectorEmulator *gratingEncoderDetector_;
	AMBasicControlDetectorEmulator *dwellTimeDetector_;
	AMDetectorGroup *allDetectorGroup_;
	AMDetectorGroup *XASDetectorGroup_;
	AMDetectorGroup *FastDetectorGroup_;

	AMControlSet *shutterControlSet_;

	AMControlSet *criticalControlsSet_;
	AMDetectorSet *criticalDetectorSet_;

	AMControlSet *beamOnControlSet_;

	AMControlSet *fluxResolutionSet_;

	AMControlSet *trackingSet_;
	AMControlSet *ssaManipulatorSet_;
	AMControlSetSampleManipulator *sampleManipulator_;
	QList<double> ssaManipulatorSampleTolerances_;
	QList<AMControlInfoList> ssaFiducializations_;

	/// Holds a boolean for whether everything the beamline cares about has reported back as either connected or timed out ... then we've initialized
	bool beamlineIsInitialized_;

	QList<AMControlSet*> unconnectedSets_;

	/// The sample plate currently in the SSA chamber:
	AMSamplePlatePre2013* currentSamplePlate_;

	CLSSIS3820Scaler *scaler_;

	QString beamlineWarnings_;

	AMBiHashWChecking<QString, QString> amNames2pvNames_;

	double lastEnergyValue_;

	/// The SSA Manipulator X value for an in focus pair
	AMNumber xFocus_;
	/// The SSA Manipulator Y value for an in focus pair
	AMNumber yFocus_;
	/// A flag to toggle whether or not we'll keep Y in focus during X moves
	bool moveInFocus_;
};


#endif // AM_SGMBEAMLINE_H
