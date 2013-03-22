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

#include "beamline/AMOldDetector.h"
#include "beamline/AMSingleControlDetector.h"
#include "beamline/SGM/SGMMCPDetector.h"
#include "beamline/CLS/CLSPGTDetector.h"
#include "beamline/CLS/CLSOceanOptics65000Detector.h"
#include "beamline/CLS/CLSAmptekSDD123Detector.h"
#include "beamline/AMDetectorGroup.h"
#include "beamline/AMDetectorSet.h"
#include "beamline/CLS/CLSAmptekSDD123DetectorNew.h"
#include "beamline/CLS/CLSBasicScalerChannelDetector.h"
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

class SGMGratingAction;
class AMSamplePlate;
class SGMMAXvMotor;
class CLSCAEN2527HVChannel;
class CLSPGT8000HVChannel;

class SGMBeamline : public AMBeamline
{
	Q_OBJECT

public:
	SGMBeamlineInfo::sgmGrating currentGrating() const;

	static SGMBeamline* sgm();		// singleton-class accessor

	virtual ~SGMBeamline();

	bool isConnected() const;
	bool isReady() const;

	QStringList unconnectedCriticals() const;

	bool detectorConnectedByName(QString name);

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
	AMControl* undulatorTracking() const { return undulatorTracking_;}
	AMControl* monoTracking() const { return monoTracking_;}
	AMControl* exitSlitTracking() const { return exitSlitTracking_;}

	QString currentEndstation() const;

	AMOldDetector* teyDetector() const { return teyScalerDetector_;}
	AMOldDetector* tfyDetector() const { return tfyScalerDetector_;}
	AMOldDetector* pgtDetector() const { return pgtDetector_;}
	AMOldDetector* oos65000Detector() const { return oos65000Detector_;}
	AMOldDetector* i0Detector() const { return i0ScalerDetector_;}
	AMOldDetector* eVFbkDetector() const { return eVFbkDetector_;}
	AMOldDetector* photodiodeDetector() const { return photodiodeScalerDetector_;}
	AMOldDetector* encoderUpDetector() const { return encoderUpDetector_;}
	AMOldDetector* encoderDownDetector() const { return encoderDownDetector_;}
	AMOldDetector* ringCurrentDetector() const { return ringCurrentDetector_;}
	AMOldDetector* filterPD1ScalarDetector() const { return filterPD1ScalarDetector_;}
	AMOldDetector* filterPD2ScalarDetector() const { return filterPD2ScalarDetector_;}
	AMOldDetector* filterPD3ScalarDetector() const { return filterPD3ScalarDetector_;}
	AMOldDetector* filterPD4ScalarDetector() const { return filterPD4ScalarDetector_;}
	AMOldDetector* amptekSDD1() const { return amptekSDD1_;}
	bool isSDD1Enabled() const;
	AMBeamlineActionItem* createSDD1EnableAction(bool setEnabled);
	AMOldDetector* amptekSDD2() const { return amptekSDD2_;}
	bool isSDD2Enabled() const;
	AMBeamlineActionItem* createSDD2EnableAction(bool setEnabled);
	AMDetector* newAmptekSDD1() const { return newAmptekSDD1_;}
	AMDetector* newAmptekSDD2() const { return newAmptekSDD2_;}
	AMDetector* newTEYDetector() const { return newTEYDetector_;}
	AMDetector* newTFYDetector() const { return newTFYDetector_;}
	AMDetector* newI0Detector() const { return newI0Detector_;}
	AMDetector* newPDDetector() const { return newPDDetector_;}
	AMDetectorGroup *newDetectorSet() const { return newDetectorSet_;}

	AMControl* loadlockCCG() const { return loadlockCCG_;}
	AMControl* loadlockTCG() const { return loadlockTCG_;}
	SGMMAXvMotor* ssaManipulatorX() const { return ssaManipulatorX_;}
	SGMMAXvMotor* ssaManipulatorY() const { return ssaManipulatorY_;}
	SGMMAXvMotor* ssaManipulatorZ() const { return ssaManipulatorZ_;}
	SGMMAXvMotor* ssaManipulatorRot() const { return ssaManipulatorRot_;}
	AMControl* beamlineScanning() const { return beamlineScanning_;}
	AMControl* beamlineReady() const { return beamlineReady_;}
	AMControl* nextDwellTimeTrigger() const { return nextDwellTimeTrigger_;}
	AMControl* nextDwellTimeConfirmed() const { return nextDwellTimeConfirmed_;}
	AMControl* picoammeterDwellTime() const { return picoammeterDwellTime_;}
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
	AMControl *mirrorStripeSelection() const { return mirrorStripeSelection_;}
	/// Returns the mirror selection control for Carbon
	AMControl *mirrorStripeSelectCarbon() const { return mirrorStripeSelectCarbon_;}
	/// Returns the mirror selection control for Silicon
	AMControl *mirrorStripeSelectSilicon() const { return mirrorStripeSelectSilicon_;}
	/// Returns the undulator offset control (for detuning)
	AMControl *undulatorOffset() const { return undulatorOffset_;}
	/// Returns the master dwell time for the synchronized dwell time application
	AMControl *masterDwell() const { return masterDwell_;}
	CLSCAEN2527HVChannel* hvChannel106() const { return hvChannel106_;}
	CLSCAEN2527HVChannel* hvChannel109() const { return hvChannel109_;}
	CLSPGT8000HVChannel* hvChannelPGT() const { return hvChannelPGT_;}

	//CLSSynchronizedDwellTime* synchronizedDwellTime() const { return synchronizedDwellTime_;}
	virtual AMSynchronizedDwellTime* synchronizedDwellTime() const { return synchronizedDwellTime_;}
	int synchronizedDwellTimeDetectorIndex(AMOldDetector *detector) const;

	AMControlSet* fluxResolutionSet() const { return fluxResolutionSet_;}
	AMControlSet* trackingSet() const { return trackingSet_;}
	AMControlSet* ssaManipulatorSet() const { return ssaManipulatorSet_; }
	QList<AMControlInfoList> ssaFiducializations() const { return ssaFiducializations_; }

	AMControlSetSampleManipulator* sampleManipulator() const { return sampleManipulator_; }

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

	AMSamplePlate* currentSamplePlate() const { return currentSamplePlate_; }
	virtual int currentSamplePlateId() const;
	int currentSampleId();
	QString currentSampleDescription();

	AMBeamlineListAction* createBeamOnActions();
	AMBeamlineListAction* createStopMotorsAction();

	AMBeamlineListAction* createGoToTransferPositionActions();
	AMBeamlineListAction* createGoToMeasurementPositionActions();

	AMBeamlineListAction* createTransferActions(SGMBeamlineInfo::sgmTransferType transferType);
	AMBeamlineListAction* createTransferLoadLockOutActions();
	AMBeamlineListAction* createTransferLoadLockInActions();
	AMBeamlineListAction* createTransferChamberOutActions();
	AMBeamlineListAction* createTransferChamberInActions();

	AMBeamlineHighVoltageChannelToggleAction* createHV106OnActions();
	AMBeamlineHighVoltageChannelToggleAction* createHV106OffActions();
	AMBeamlineHighVoltageChannelToggleAction* createHV109OnActions();
	AMBeamlineHighVoltageChannelToggleAction* createHV109OffActions();
	AMBeamlineHighVoltageChannelToggleAction* createHVPGTOnActions();
	AMBeamlineHighVoltageChannelToggleAction* createHVPGTOffActions();

	/// Returns a pointer to the scaler IF the scaler IS connected
	CLSSIS3820Scaler* scaler();
	/// Returns a pointer to the scaler EVEN IF the scaler ISN'T yet connected
	CLSSIS3820Scaler* rawScaler();

	bool isBeamlineScanning();

	virtual AMControlSet* currentSamplePositioner() { return ssaManipulatorSet(); }
	virtual QList<AMControlInfoList> currentFiducializations() { return ssaFiducializations(); }

	bool isVisibleLightOn() const;

	bool energyValidForSettings(SGMBeamlineInfo::sgmGrating grating, SGMBeamlineInfo::sgmHarmonic harmonic, double energy);
	bool energyRangeValidForSettings(SGMBeamlineInfo::sgmGrating grating, SGMBeamlineInfo::sgmHarmonic harmonic, double minEnergy, double maxEnergy);

	QList< QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic> > gratingHarmonicForEnergyRange(double minEnergy, double maxEnergy);
	QPair<double, double> energyRangeForGratingHarmonic(SGMBeamlineInfo::sgmGrating grating, SGMBeamlineInfo::sgmHarmonic harmonic);

	QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic> forBestFlux(double minEnergy, double maxEnergy) const;
	QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic> forBestResolution(double minEnergy, double maxEnergy) const;

	/// Returns back the list of detectors that this set has registered against it. They may not be in the set yet, because they're not connected (or not yet connected on startup)
	QList<AMOldDetector*> possibleDetectorsForSet(AMOldDetectorSet *set);

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
	AMControl *loadlockCCG_;
	AMControl *loadlockTCG_;
	SGMMAXvMotor *ssaManipulatorX_;
	SGMMAXvMotor *ssaManipulatorY_;
	SGMMAXvMotor *ssaManipulatorZ_;
	SGMMAXvMotor *ssaManipulatorRot_;
	AMControl *beamlineScanning_;
	AMControl *beamlineReady_;
	AMControl *nextDwellTimeTrigger_;
	AMControl *nextDwellTimeConfirmed_;
	AMControl *picoammeterDwellTime_;
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
	AMOldDetector* amptekSDD1_;
	AMOldDetector* amptekSDD2_;
	CLSAmptekSDD123DetectorNew *newAmptekSDD1_;
	CLSAmptekSDD123DetectorNew *newAmptekSDD2_;
	CLSBasicScalerChannelDetector *newTEYDetector_;
	CLSBasicScalerChannelDetector *newTFYDetector_;
	CLSBasicScalerChannelDetector *newI0Detector_;
	CLSBasicScalerChannelDetector *newPDDetector_;
	AMDetectorGroup *newDetectorSet_;

	AMControlSet *criticalControlsSet_;
	AMOldDetectorSet *criticalDetectorsSet_;
	AMOldDetectorSet *rawDetectorsSet_;

	AMControlSet *beamOnControlSet_;
	AMControlSet *transferLoadLockOutControlSet_;
	AMControlSet *transferLoadLockInControlSet_;

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

	AMOrderedSet<QString, QPixmap> transferLoadLockOutAction1Help_;
	AMOrderedSet<QString, QPixmap> transferLoadLockOutAction2Help_;
	AMOrderedSet<QString, QPixmap> transferLoadLockOutAction3Help_;
	AMOrderedSet<QString, QPixmap> transferLoadLockOutAction4Help_;
	AMOrderedSet<QString, QPixmap> transferLoadLockOutAction5Help_;

	AMOrderedSet<QString, QPixmap> transferLoadLockInAction2Help_;
	AMOrderedSet<QString, QPixmap> transferLoadLockInAction3Help_;
	AMOrderedSet<QString, QPixmap> transferLoadLockInAction4Help_;
	AMOrderedSet<QString, QPixmap> transferLoadLockInAction5Help_;
	AMOrderedSet<QString, QPixmap> transferLoadLockInAction6Help_;

	AMOrderedSet<QString, QPixmap> transferChamberOutAction1Help_;
	AMOrderedSet<QString, QPixmap> transferChamberOutAction3Help_;
	AMOrderedSet<QString, QPixmap> transferChamberOutAction5Help_;
	AMOrderedSet<QString, QPixmap> transferChamberOutAction6Help_;
	AMOrderedSet<QString, QPixmap> transferChamberOutAction7Help_;
	AMOrderedSet<QString, QPixmap> transferChamberOutAction8Help_;

	AMOrderedSet<QString, QPixmap> transferChamberInAction1Help_;
	AMOrderedSet<QString, QPixmap> transferChamberInAction2Help_;
	AMOrderedSet<QString, QPixmap> transferChamberInAction3Help_;
	AMOrderedSet<QString, QPixmap> transferChamberInAction4Help_;

	QString beamlineWarnings_;

	AMBiHash<QString, QString> amNames2pvNames_;

	double lastEnergyValue_;

	friend class SGMGratingAction;

private:
	void usingSGMBeamline();
};


#endif // ACQMAN_SGMBEAMLINE_H
