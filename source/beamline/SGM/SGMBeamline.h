/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef ACQMAN_SGMBEAMLINE_H
#define ACQMAN_SGMBEAMLINE_H

#include "beamline/AMBeamline.h"
#include "dataman/SGM/SGMBeamlineInfo.h"

#include "beamline/AMDetectorGroup.h"
#include "beamline/AMDetectorSet.h"
#include "beamline/AMControlSet.h"
#include "util/AMBiHash.h"
#include "actions/AMBeamlineControlAction.h"
#include "actions/AMBeamlineControlMoveAction.h"
#include "actions/AMBeamlineControlSetMoveAction.h"
#include "actions/AMBeamlineControlWaitAction.h"
#include "actions/AMBeamlineControlStopAction.h"
#include "actions/AMBeamlineUserConfirmAction.h"
#include "actions/AMBeamlineHighVoltageChannelToggleAction.h"
#include "actions/AMBeamlineActionsList.h"
#include "actions/AMBeamlineParallelActionsList.h"
#include "actions/AMBeamlineListAction.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/AMControlSetSampleManipulator.h"
#include "beamline/AMOldDetectorSet.h"
#include "beamline/AMControlOptimization.h"
#include "beamline/CLS/CLSSynchronizedDwellTime.h"

#define SGMBEAMLINE_PV_NAME_LOOKUPS_FAILED 312001

class AMSamplePlate;
class SGMMAXvMotor;
class CLSCAEN2527HVChannel;
class CLSPGT8000HVChannel;

class AMOldDetector;
class CLSAmptekSDD123DetectorNew;
class CLSPGTDetectorV2;
class CLSQE65000Detector;
class CLSAdvancedScalerChannelDetector;
class AMBasicControlDetectorEmulator;

class SGMBeamline : public AMBeamline
{
Q_OBJECT
public:

	static SGMBeamline* sgm();		// singleton-class accessor
	virtual ~SGMBeamline();

	bool isConnected() const;
	bool isReady() const;
    bool isBeamlineScanning() const;
	bool isVisibleLightOn() const;

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






	CLSCAEN2527HVChannel* hvChannel106() const { return hvChannel106_;}

	CLSCAEN2527HVChannel* hvChannel109() const { return hvChannel109_;}
	CLSPGT8000HVChannel* hvChannelPGT() const { return hvChannelPGT_;}

	virtual AMSynchronizedDwellTime* synchronizedDwellTime() const { return synchronizedDwellTime_;}

	int synchronizedDwellTimeDetectorIndex(AMOldDetector *detector) const;

	/// Returns the validity of an action (see AMBeamline::ActionValidity). Currently the SGM responds that old XAS and Fast scans are AMBeamline::ActionNeverValid.
	virtual AMAction3::ActionValidity validateAction(AMAction3 *action);
	/// Returns messages for invalid actions (old scan configurations) and "Action is Currently Valid" for all other actions.
	virtual QString validateActionMessage(AMAction3 *action);

	SGMBeamlineInfo::sgmGrating currentGrating() const;
	QString currentEndstation() const;

	/// Returns a pointer to the scaler IF the scaler IS connected
	CLSSIS3820Scaler* scaler();
	/// Returns a pointer to the scaler EVEN IF the scaler ISN'T yet connected
	CLSSIS3820Scaler* rawScaler();

	AMControlSetSampleManipulator* sampleManipulator() const { return sampleManipulator_; }
	virtual AMControlSet* currentSamplePositioner() { return ssaManipulatorSet(); }
	virtual QList<AMControlInfoList> currentFiducializations() { return ssaFiducializations(); }

	AMSamplePlate* currentSamplePlate() const { return currentSamplePlate_; }
	virtual int currentSamplePlateId() const;
	int currentSampleId();
	QString currentSampleDescription();

	AMControlSet* fluxResolutionSet() const { return fluxResolutionSet_;}
	AMControlSet* trackingSet() const { return trackingSet_;}
	AMControlSet* ssaManipulatorSet() const { return ssaManipulatorSet_; }
	QList<AMControlInfoList> ssaFiducializations() const { return ssaFiducializations_; }

	AMOldDetector* teyDetector() const;
	AMOldDetector* tfyDetector() const;
	//AMOldDetector* pgtDetector() const;
	AMOldDetector* oos65000Detector() const;
	AMOldDetector* i0Detector() const;
	AMOldDetector* eVFbkDetector() const;
	AMOldDetector* photodiodeDetector() const;
	AMOldDetector* encoderUpDetector() const;
	AMOldDetector* encoderDownDetector() const;
	AMOldDetector* ringCurrentDetector() const;
	AMOldDetector* filterPD1ScalarDetector() const;
	AMOldDetector* filterPD2ScalarDetector() const;
	AMOldDetector* filterPD3ScalarDetector() const;
	AMOldDetector* filterPD4ScalarDetector() const;
	/*
	AMOldDetector* amptekSDD1() const;
	AMOldDetector* amptekSDD2() const;
	*/
	AMDetector* newAmptekSDD1() const;
	AMDetector* newAmptekSDD2() const;
	AMDetector* newAmptekSDD3() const;
	AMDetector* newAmptekSDD4() const;
	AMDetector* newAmptekSDD5() const;
	//AMDetector* newPGTDetector() const;
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

	/*
	bool isSDD1Enabled() const;
	bool isSDD2Enabled() const;
	AMBeamlineActionItem* createSDD1EnableAction(bool setEnabled);
	AMBeamlineActionItem* createSDD2EnableAction(bool setEnabled);
	*/

	AMDetectorGroup *newDetectorSet() const { return newDetectorSet_;}
	AMDetectorGroup *XASDetectorGroup() const { return XASDetectorGroup_;}
	AMDetectorGroup *FastDetectorGroup() const { return FastDetectorGroup_;}

	bool detectorConnectedByName(QString name);

	/// Critical detectors that must be there for the beamline to be considered "connected". Can be altered in the beamline settings view
	AMOldDetectorSet* criticalDetectorsSet() const { return criticalDetectorsSet_;}
	/// All of the detectors on the beamline, regardless of whether they're connnected or not
	AMOldDetectorSet* rawDetectors() const { return rawDetectorsSet_;}

	/// All of the detectors currently connected on the beamline
	AMOldDetectorSet* allDetectors() const { return allDetectors_;}
	/// List of connected feedback detectors
	AMOldDetectorSet* feedbackDetectors() const { return feedbackDetectors_;}
	/// List of connected detectors availabe for XAS scans
	AMOldDetectorSet* XASDetectors() const { return XASDetectors_;}
	/// List of connected detectors available for Fast scans
	AMOldDetectorSet* FastDetectors() const { return FastDetectors_;}

	/// Returns back the list of detectors that this set has registered against it. They may not be in the set yet, because they're not connected (or not yet connected on startup)
	QList<AMOldDetector*> possibleDetectorsForSet(AMOldDetectorSet *set);

	AMBeamlineListAction* createBeamOnActions();
	AMAction3 *createBeamOnActions3();
	AMBeamlineListAction* createStopMotorsAction();

	AMBeamlineListAction* createGoToTransferPositionActions();
	AMBeamlineListAction* createGoToMeasurementPositionActions();

	AMBeamlineHighVoltageChannelToggleAction* createHV106OnActions();
	AMBeamlineHighVoltageChannelToggleAction* createHV106OffActions();
	AMBeamlineHighVoltageChannelToggleAction* createHV109OnActions();
	AMBeamlineHighVoltageChannelToggleAction* createHV109OffActions();
	AMBeamlineHighVoltageChannelToggleAction* createHVPGTOnActions();
	AMBeamlineHighVoltageChannelToggleAction* createHVPGTOffActions();

public slots:
	void setCurrentSamplePlate(AMSamplePlate *newSamplePlate);

	void visibleLightOn();
	void visibleLightOff();

	void closeVacuum();

	void setCurrentEndstation(SGMBeamlineInfo::sgmEndstation endstation);
	void setCurrentMirrorStripe(SGMBeamlineInfo::sgmMirrorStripe mirrorStripe);

signals:
	void beamlineScanningChanged(bool scanning);
	void controlSetConnectionsChanged();
	void criticalControlsConnectionsChanged();
	void criticalConnectionsChanged();
	void beamlineReadyChanged();

	void visibleLightStatusChanged(const QString& status);
	void beamlineWarningsChanged(const QString& warnings);
	void currentSamplePlateChanged(AMSamplePlate *newSamplePlate);

	void currentEndstationChanged(SGMBeamlineInfo::sgmEndstation);
	void currentMirrorStripeChanged(SGMBeamlineInfo::sgmMirrorStripe);

	void detectorHVChanged();
	void detectorAvailabilityChanged(AMOldDetector *detector, bool available);

	void beamlineInitialized();

protected slots:
	void onBeamlineScanningValueChanged(double value);
	void onControlSetConnected(bool csConnected);
	void onCriticalControlsConnectedChanged(bool isConnected, AMControl *controll);
	void onCriticalsConnectedChanged();
	void onEnergyValueChanged();

	void onActiveEndstationChanged(double value);
	void onMirrorStripeChanged(double value);

	void recomputeWarnings();

	void onVisibleLightChanged(double value);
	void onDetectorAvailabilityChanged(AMOldDetector *detector, bool isAvailable);
	void ensureDetectorTimeout();

	void computeBeamlineInitialized();

protected:
	void setupControls();
	void setupNameToPVLookup();

	/// Sets up the exposed controls for the SGM beamline (accessible through AMControlMoveAction)
	void setupExposedControls();

	/// Sets up the exposed detectors for the SGM beamline (accessible through the AMScanConfiguration/Controller interface)
	void setupExposedDetectors();

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
	AMControl *monoTracking_;
	AMControl *exitSlitTracking_;
	AMControl *tfyHV_;
	AMControl *tfyHVToggle_;
	CLSCAEN2527HVChannel *hvChannel106_;
	CLSCAEN2527HVChannel *hvChannel109_;
	CLSPGT8000HVChannel *hvChannelPGT_;
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


	AMOldDetector *teyScalerDetector_;
	AMOldDetector *tfyScalerDetector_;
	AMOldDetector *pgtDetector_;
	AMOldDetector *oos65000Detector_;
	AMOldDetector *i0ScalerDetector_;
	AMOldDetector *eVFbkDetector_;
	AMOldDetector *photodiodeScalerDetector_;
	AMOldDetector *encoderUpDetector_;
	AMOldDetector *encoderDownDetector_;
	AMOldDetector *ringCurrentDetector_;
	AMOldDetector *filterPD1ScalarDetector_;
	AMOldDetector *filterPD2ScalarDetector_;
	AMOldDetector *filterPD3ScalarDetector_;
	AMOldDetector *filterPD4ScalarDetector_;
	//AMOldDetector* amptekSDD1_;
	//AMOldDetector* amptekSDD2_;
	CLSAmptekSDD123DetectorNew *newAmptekSDD1_;
	CLSAmptekSDD123DetectorNew *newAmptekSDD2_;
	CLSAmptekSDD123DetectorNew *newAmptekSDD3_;
	CLSAmptekSDD123DetectorNew *newAmptekSDD4_;
	CLSAmptekSDD123DetectorNew *newAmptekSDD5_;
	//CLSPGTDetectorV2 *newPGTDetector_;
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
	AMDetectorGroup *newDetectorSet_;
	AMDetectorGroup *XASDetectorGroup_;
	AMDetectorGroup *FastDetectorGroup_;

	AMControlSet *criticalControlsSet_;
	AMOldDetectorSet *criticalDetectorsSet_;
	AMOldDetectorSet *rawDetectorsSet_;

	AMControlSet *beamOnControlSet_;

	AMControlOptimization *fluxOptimization_;
	AMControlOptimization *resolutionOptimization_;
	AMControlSet *fluxResolutionSet_;

	AMControlSet *trackingSet_;
	AMControlSet *ssaManipulatorSet_;
	AMControlSetSampleManipulator *sampleManipulator_;
	QList<double> ssaManipulatorSampleTolerances_;
	QList<AMControlInfoList> ssaFiducializations_;

	AMOldDetectorSet *allDetectors_;
	AMOldDetectorSet *feedbackDetectors_;
	AMOldDetectorSet *XASDetectors_;
	AMOldDetectorSet *FastDetectors_;

	/// Mapping detectors to their sets and whether they are default or not
	QMultiMap<AMOldDetector*, QPair<AMOldDetectorSet*, bool> > *detectorMap_;
	/// Generally listing all detectors this beamline can have
	QList<AMOldDetector*> detectorRegistry_;
	/// Listing the detectors that haven't responded (either as connected or timed out)
	QList<AMOldDetector*> unrespondedDetectors_;

	/// Holds a boolean for whether everything the beamline cares about has reported back as either connected or timed out ... then we've initialized
	bool beamlineIsInitialized_;

	QList<AMControlSet*> unconnectedSets_;

	/// The sample plate currently in the SSA chamber:
	AMSamplePlate* currentSamplePlate_;

	CLSSIS3820Scaler *scaler_;

	QString beamlineWarnings_;

	AMBiHashWChecking<QString, QString> amNames2pvNames_;

	double lastEnergyValue_;
};


#endif // ACQMAN_SGMBEAMLINE_H
