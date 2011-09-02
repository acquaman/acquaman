/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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
#include "beamline/SGM/SGMBeamlineInfo.h"

#include "beamline/AMDetector.h"
#include "beamline/AMSingleControlDetector.h"
#include "beamline/MCPDetector.h"
#include "beamline/PGTDetector.h"
#include "beamline/OceanOptics65000Detector.h"
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

#include "beamline/AMDetectorSet.h"

#include "acquaman/AMControlOptimization.h"


class SGMGratingAction;
class AMSamplePlate;
class CLSVMEMotor;
class CLSCAEN2527HVChannel;
class CLSPGT8000HVChannel;

class SGMBeamline : public AMBeamline
{
	Q_OBJECT

public:
	enum sgmGrating{
		lowGrating = 0,
		mediumGrating = 1,
		highGrating = 2
	};
	QString sgmGratingName(SGMBeamline::sgmGrating grating) const;
	QString sgmGratingDescription(SGMBeamline::sgmGrating grating) const;
	SGMEnergyParameters* energyParametersForGrating(SGMBeamline::sgmGrating grating) const;
	SGMBeamline::sgmGrating currentGrating() const;

	enum sgmHarmonic{
		firstHarmonic = 0,
		thirdHarmonic = 1
	};
	QString sgmHarmonicName(SGMBeamline::sgmHarmonic harmonic) const;
	QString sgmHarmonicDescription(SGMBeamline::sgmHarmonic harmonic) const;

	enum sgmDetectorSignalSource{
		picoammeters = 0,
		scaler = 1
	};
	QString sgmDetectorSignalSourceName(SGMBeamline::sgmDetectorSignalSource dss) const;

	enum sgmEndstation{
		scienta = 0,
		ssa = 1
	};
	QString sgmEndstationName(SGMBeamline::sgmEndstation endstation) const;

	enum sgmTransferType{
		loadlockOut = 1,
		loadlockIn,
		ChamberOut,
		ChamberIn
	};

	static SGMBeamline* sgm();		// singleton-class accessor

	virtual ~SGMBeamline();

	bool isConnected() const {
		return criticalControlsSet_->isConnected();
	}

	QStringList unconnectedCriticals() const;

	bool detectorConnectedByName(QString name);

	QString beamlineWarnings();

	QString pvName(const QString &amName) const { return amNames2pvNames_.valueF(amName);}
	QString amName(const QString &pvName) const { return amNames2pvNames_.valueR(pvName);}

	AMControl* ringCurrent() const { return ringCurrent_; }
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

	QString detectorSignalSource() const {
		if(detectorSignalSource_->value() == 0)
			return sgmDetectorSignalSourceName(SGMBeamline::picoammeters);
		else if(detectorSignalSource_->value() == 1)
			return sgmDetectorSignalSourceName(SGMBeamline::scaler);
		else
			return sgmDetectorSignalSourceName((SGMBeamline::sgmDetectorSignalSource)272727);
	}

	QString currentEndstation() const{
		if(activeEndstation_->value() == 0)
			return sgmEndstationName(SGMBeamline::scienta);
		else if(activeEndstation_->value() == 1)
			return sgmEndstationName(SGMBeamline::ssa);
		else
			return sgmEndstationName((SGMBeamline::sgmEndstation)272727);
	}

	AMDetector* teyDetector() const {
		if(detectorSignalSource_->value() == 0)
			return teyPicoDetector_;
		else
			return teyScalerDetector_;
	}
	AMDetector* tfyDetector() const {
		if(detectorSignalSource_->value() == 0)
			return tfyPicoDetector_;
		else
			return tfyScalerDetector_;
	}
	AMDetector* pgtDetector() const { return pgtDetector_;}
	AMDetector* oos65000Detector() const { return oos65000Detector_;}
	AMDetector* i0Detector() const {
		if(detectorSignalSource_->value() == 0)
			return i0PicoDetector_;
		else
			return i0ScalerDetector_;
	}
	AMDetector* eVFbkDetector() const { return eVFbkDetector_;}
	AMDetector* photodiodeDetector() const {
		if(detectorSignalSource_->value() == 0)
			return photodiodePicoDetector_;
		else
			return photodiodeScalerDetector_;
	}
	AMDetector* encoderUpDetector() const { return encoderUpDetector_;}
	AMDetector* encoderDownDetector() const { return encoderDownDetector_;}

	bool detectorValidForCurrentSignalSource(AMDetector *detector);
	bool detectorValidForCurrentSignalSource(AMDetectorInfo *detectorInfo);
	bool usingPicoammeterSource();
	bool usingScalerSource();

	AMControl* loadlockCCG() const { return loadlockCCG_;}
	AMControl* loadlockTCG() const { return loadlockTCG_;}
	CLSVMEMotor* ssaManipulatorX() const { return ssaManipulatorX_;}
	CLSVMEMotor* ssaManipulatorY() const { return ssaManipulatorY_;}
	CLSVMEMotor* ssaManipulatorZ() const { return ssaManipulatorZ_;}
	CLSVMEMotor* ssaManipulatorRot() const { return ssaManipulatorRot_;}
	AMControl* beamlineScanning() const { return beamlineScanning_;}
	AMControl* beamlineReady() const { return beamlineReady_;}
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
	AMControl* scalerStart() const { return scalerStart_;}
	AMControl* scalerIntegrationTime() const { return scalerIntegrationTime_;}
	AMControl* scalerScansPerBuffer() const { return scalerScansPerBuffer_;}
	AMControl* scalerTotalNumberOfScans() const { return scalerTotalNumberOfScans_;}
	AMControl* scalerMode() const { return scalerMode_;}
	AMControl* ssaIllumination() const { return ssaIllumination_;}
	AMControl* tfyHVToggle() const { return tfyHVToggle_;}
	CLSCAEN2527HVChannel* hvChannel106() const { return hvChannel106_;}
	CLSCAEN2527HVChannel* hvChannel109() const { return hvChannel109_;}
	CLSPGT8000HVChannel* hvChannelPGT() const { return hvChannelPGT_;}


	AMControlSet* fluxResolutionSet() const { return fluxResolutionSet_;}
	AMControlSet* trackingSet() const { return trackingSet_;}
	AMControlSet* ssaManipulatorSet() const { return ssaManipulatorSet_; }
	QList<AMControlInfoList> ssaFiducializations() const { return ssaFiducializations_; }

	AMDetectorSet* allDetectors() const { return allDetectors_;}
	AMDetectorSet* feedbackDetectors() const { return feedbackDetectors_;}
	AMDetectorSet* XASDetectors() const { return XASDetectors_;}
	AMDetectorSet* FastDetectors() const { return FastDetectors_;}

	AMSamplePlate* currentSamplePlate() const { return currentSamplePlate_; }
	int currentSampleId();
	QString currentSampleDescription();

	AMBeamlineListAction* createBeamOnActions();
	AMBeamlineListAction* createStopMotorsAction();

	AMBeamlineListAction* createGoToTransferPositionActions();
	AMBeamlineListAction* createGoToMeasurementPositionActions();

	AMBeamlineListAction* createTransferActions(SGMBeamline::sgmTransferType transferType);
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

	bool isBeamlineScanning();

	virtual AMControlSet* currentSamplePositioner() { return ssaManipulatorSet(); }
	virtual QList<AMControlInfoList> currentFiducializations() { return ssaFiducializations(); }

	bool isVisibleLightOn();

	bool energyValidForSettings(sgmGrating grating, sgmHarmonic harmonic, double energy);
	bool energyRangeValidForSettings(sgmGrating grating, sgmHarmonic harmonic, double minEnergy, double maxEnergy);

	QList< QPair<sgmGrating, sgmHarmonic> > gratingHarmonicForEnergyRange(double minEnergy, double maxEnergy);
	QPair<double, double> energyRangeForGratingHarmonic(sgmGrating grating, sgmHarmonic harmonic);

	QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic> forBestFlux(double minEnergy, double maxEnergy) const;
	QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic> forBestResolution(double minEnergy, double maxEnergy) const;

public slots:
	void setCurrentSamplePlate(AMSamplePlate *newSamplePlate);

	void visibleLightOn();
	void visibleLightOff();

	void closeVacuum();

	void setDetectorSignalSource(SGMBeamline::sgmDetectorSignalSource detectorSignalSource){
		if(detectorSignalSource == SGMBeamline::picoammeters)
			detectorSignalSource_->move(0);
		else if(detectorSignalSource == SGMBeamline::scaler)
			detectorSignalSource_->move(1);
		return;
	}

	void setCurrentEndstation(SGMBeamline::sgmEndstation endstation){
		if(endstation == SGMBeamline::scienta)
			activeEndstation_->move(0);
		else if(endstation == SGMBeamline::ssa)
			activeEndstation_->move(1);
		return;
	}

signals:
	void beamlineScanningChanged(bool scanning);
	void controlSetConnectionsChanged();
	void criticalControlsConnectionsChanged();

	void visibleLightStatusChanged(const QString& status);

	void beamlineWarningsChanged(const QString& warnings);

	void currentSamplePlateChanged(AMSamplePlate *newSamplePlate);

	void detectorSignalSourceChanged(SGMBeamline::sgmDetectorSignalSource);
	void currentEndstationChanged(SGMBeamline::sgmEndstation);

	void detectorHVChanged();

protected slots:
	void onBeamlineScanningValueChanged(double value);
	void onControlSetConnected(bool csConnected);
	void onCriticalControlsConnectedChanged(bool isConnected, AMControl *controll);

	void onDetectorSignalSourceChanged(double value);
	void onActiveEndstationChanged(double value);

	void recomputeWarnings();

	void onVisibleLightChanged(double value);

protected:
	// Singleton implementation:
	SGMBeamline();					// protected constructor... only access through Beamline::bl()

	SGMBeamlineInfo *infoObject_;

	// Parts of this beamline:
	///////////////////////////////

	AMControl *ringCurrent_;
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
	AMControl *teyPico_;
	AMControl *teyScaler_;
	AMControl *tfyPico_;
	AMControl *tfyScaler_;
	AMControl *tfyHV_;
	AMControl *tfyHVToggle_;
	CLSCAEN2527HVChannel *hvChannel106_;
	CLSCAEN2527HVChannel *hvChannel109_;
	CLSPGT8000HVChannel *hvChannelPGT_;
	AMControl *pgt_;
	AMControl *pgtHV_;
	AMControl *pgtIntegrationTime_;
	AMControl *pgtIntegrationMode_;
	AMControl *oos65000_;
	AMControl *oos65000IntegrationTime_;
	AMControl *i0Pico_;
	AMControl *i0Scaler_;
	AMControl *eVFbk_;
	AMControl *photodiodePico_;
	AMControl *photodiodeScaler_;
	AMControl *encoderUp_;
	AMControl *encoderDown_;
	AMControl *loadlockCCG_;
	AMControl *loadlockTCG_;
	CLSVMEMotor *ssaManipulatorX_;
	CLSVMEMotor *ssaManipulatorY_;
	CLSVMEMotor *ssaManipulatorZ_;
	CLSVMEMotor *ssaManipulatorRot_;
	AMControl *beamlineScanning_;
	AMControl *beamlineReady_;
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
	AMControl *scalerStart_;
	AMControl *scalerIntegrationTime_;
	AMControl *scalerScansPerBuffer_;
	AMControl *scalerTotalNumberOfScans_;
	AMControl *scalerMode_;
	AMControl *detectorSignalSource_;
	AMControl *ssaIllumination_;

	AMControlSet *teyPicoControlSet_;
	AMDetector *teyPicoDetector_;
	AMControlSet *teyScalerControlSet_;
	AMDetector *teyScalerDetector_;
	AMControlSet *tfyPicoControlSet_;
	AMDetector *tfyPicoDetector_;
	AMControlSet *tfyScalerControlSet_;
	AMDetector *tfyScalerDetector_;
	AMControlSet *pgtControlSet_;
	AMDetector *pgtDetector_;
	AMControlSet *oos65000ControlSet_;
	AMDetector *oos65000Detector_;
	AMDetector *i0PicoDetector_;
	AMControlSet *i0PicoControlSet_;
	AMDetector *i0ScalerDetector_;
	AMControlSet *i0ScalerControlSet_;
	AMDetector *eVFbkDetector_;
	AMControlSet *eVFbkControlSet_;
	AMDetector *photodiodePicoDetector_;
	AMControlSet *photodiodePicoControlSet_;
	AMDetector *photodiodeScalerDetector_;
	AMControlSet *photodiodeScalerControlSet_;
	AMDetector *encoderUpDetector_;
	AMControlSet *encoderUpControlSet_;
	AMDetector *encoderDownDetector_;
	AMControlSet *encoderDownControlSet_;

	AMControlSet *criticalControlsSet_;
	AMControlSet *beamOnControlSet_;
	AMControlSet *transferLoadLockOutControlSet_;
	AMControlSet *transferLoadLockInControlSet_;

	AMControlOptimization *fluxOptimization_;
	AMControlOptimization *resolutionOptimization_;
	AMControlSet *fluxResolutionSet_;

	AMControlSet *trackingSet_;
	AMControlSet *ssaManipulatorSet_;
	QList<double> ssaManipulatorSampleTolerances_;
	QList<AMControlInfoList> ssaFiducializations_;

	AMDetectorSet *allDetectors_;
	AMDetectorSet *feedbackDetectors_;
	AMDetectorSet *XASDetectors_;
	AMDetectorSet *FastDetectors_;

	QList<AMControlSet*> unconnectedSets_;

	/// The sample plate currently in the SSA chamber:
	AMSamplePlate* currentSamplePlate_;

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

	friend class SGMGratingAction;

private:
	void usingSGMBeamline();
	void usingFakeBeamline();
};

class SGMFluxOptimization : public AMControlOptimization
{
	Q_OBJECT
public:
	SGMFluxOptimization(QObject *parent=0);

	virtual QMap<double, double> curve(QList<QVariant> stateParameters, AMRegionsList* contextParameters);
	QMap< QString, QMap<double, double> > collapse(AMRegionsList *contextParameters);

protected:
	double collapser(QMap<double, double> optCurve);
	double maximumEnergy(AMRegionsList* regions);
	double minimumEnergy(AMRegionsList* regions);
};

class SGMResolutionOptimization : public AMControlOptimization
{
	Q_OBJECT
public:
	SGMResolutionOptimization(QObject *parent=0);

	QMap<double, double> curve(QList<QVariant> stateParameters, AMRegionsList* contextParameters);
	QMap< QString, QMap<double, double> > collapse(AMRegionsList *contextParameters);

protected:
	double collapser(QMap<double, double> optCurve);
	double maximumEnergy(AMRegionsList* regions);
	double minimumEnergy(AMRegionsList* regions);
};


#endif // ACQMAN_SGMBEAMLINE_H
