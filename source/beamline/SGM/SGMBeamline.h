/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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
#include "beamline/AMDetector.h"
#include "beamline/AMSingleControlDetector.h"
#include "beamline/MCPDetector.h"
#include "beamline/PGTDetector.h"
#include "beamline/AMControlSet.h"
#include "util/AMBiHash.h"
#include "beamline/AMBeamlineControlAction.h"
#include "beamline/AMBeamlineControlMoveAction.h"
#include "beamline/AMBeamlineControlSetMoveAction.h"
#include "beamline/AMBeamlineControlWaitAction.h"
#include "beamline/AMBeamlineUserConfirmAction.h"
#include "beamline/AMBeamlineActionsList.h"
#include "beamline/AMBeamlineParallelActionsList.h"
#include "beamline/AMBeamlineListAction.h"

#include "beamline/AMDetectorSet.h"

#include "acquaman/AMControlOptimization.h"


class SGMGratingAction;
/*
class SGMTransferAction1;
class SGMTransferAction2;
class SGMTransferAction3;
class SGMTransferAction4;
*/

class AMSamplePlate;

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

	enum sgmHarmonic{
		firstHarmonic = 1,
		thirdHarmonic = 3
	};
	QString sgmHarmonicName(SGMBeamline::sgmHarmonic harmonic) const;

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
	AMControl* undulatorTracking() const { return undulatorTracking_;}
	AMControl* monoTracking() const { return monoTracking_;}
	AMControl* exitSlitTracking() const { return exitSlitTracking_;}

	AMDetector* teyDetector() const { return teyDetector_;}
	AMDetector* tfyDetector() const { return tfyDetector_;}
	AMDetector* pgtDetector() const { return pgtDetector_;}
	AMDetector* i0Detector() const { return i0Detector_;}
	AMDetector* eVFbkDetector() const { return eVFbkDetector_;}
	AMDetector* photodiodeDetector() const { return photodiodeDetector_;}
	AMDetector* encoderUpDetector() const { return encoderUpDetector_;}
	AMDetector* encoderDownDetector() const { return encoderDownDetector_;}

	AMControl* loadlockCCG() const { return loadlockCCG_;}
	AMControl* loadlockTCG() const { return loadlockTCG_;}
	AMControl* ssaManipulatorX() const { return ssaManipulatorX_;}
	AMControl* ssaManipulatorXStop() const { return ssaManipulatorXStop_;}
	AMControl* ssaManipulatorY() const { return ssaManipulatorY_;}
	AMControl* ssaManipulatorYStop() const { return ssaManipulatorYStop_;}
	AMControl* ssaManipulatorZ() const { return ssaManipulatorZ_;}
	AMControl* ssaManipulatorZStop() const { return ssaManipulatorZStop_;}
	AMControl* ssaManipulatorRot() const { return ssaManipulatorRot_;}
	AMControl* ssaManipulatorRotStop() const { return ssaManipulatorRotStop_;}
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
	AMControl* scalerIntegrationTime() const { return scalerIntegrationTime_;}
	AMControl* scalerScansPerBuffer() const { return scalerScansPerBuffer_;}
	AMControl* scalerTotalNumberOfScans() const { return scalerTotalNumberOfScans_;}
	AMControl* scalerMode() const { return scalerMode_;}


	AMControlSet* fluxResolutionSet() const { return fluxResolutionSet_;}
	AMControlSet* trackingSet() const { return trackingSet_;}
	AMControlSet* ssaManipulatorSet() const { return ssaManipulatorSet_; }

	AMDetectorSet* allDetectors() const { return allDetectors_;}
	AMDetectorSet* feedbackDetectors() const { return feedbackDetectors_;}
	AMDetectorSet* XASDetectors() const { return XASDetectors_;}

	AMSamplePlate* currentSamplePlate() const { return currentSamplePlate_; }

	AMBeamlineControlMoveAction* beamOnAction();
	AMBeamlineParallelActionsList* beamOnActionsList();

	AMBeamlineParallelActionsList* stopMotorsActionsList();

	AMBeamlineListAction* createTransferActions(SGMBeamline::sgmTransferType transferType);
	AMBeamlineListAction* createTransferLoadLockOutActions();
	AMBeamlineListAction* createTransferLoadLockInActions();
	AMBeamlineListAction* createTransferChamberOutActions();
	AMBeamlineListAction* createTransferChamberInActions();

	bool isBeamlineScanning();

	bool isVisibleLightOn();

	bool energyValidForSettings(sgmGrating grating, sgmHarmonic harmonic, double energy);
	bool energyRangeValidForSettings(sgmGrating grating, sgmHarmonic harmonic, double minEnergy, double maxEnergy);

	QList< QPair<sgmGrating, sgmHarmonic> > gratingHarmonicForEnergyRange(double minEnergy, double maxEnergy);
	QPair<double, double> energyRangeForGratingHarmonic(sgmGrating grating, sgmHarmonic harmonic);

signals:
	void beamlineScanningChanged(bool scanning);
	void controlSetConnectionsChanged();
	void criticalControlsConnectionsChanged();

	void visibleLightStatusChanged(const QString& status);

	void beamlineWarningsChanged(const QString& warnings);

public slots:
	void visibleLightOn();
	void visibleLightOff();

	void closeVacuum();

protected slots:
	void onBeamlineScanningValueChanged(double value);
	void onControlSetConnected(bool csConnected);
	void onCriticalControlsConnectedChanged(bool isConnected, AMControl *controll);

	void recomputeWarnings();

	void createBeamOnActions();
	void onBeamOnActionsFinsihed();

	void createStopMotorsActions();
	void onStopMotorsActionsFinished();

	void onVisibleLightChanged(double value);

protected:
	// Singleton implementation:
	SGMBeamline();					// protected constructor... only access through Beamline::bl()

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
	AMControl *undulatorTracking_;
	AMControl *monoTracking_;
	AMControl *exitSlitTracking_;
	AMControl *tey_;
	AMControl *tfy_;
	AMControl *tfyHV_;
	AMControl *pgt_;
	AMControl *pgtHV_;
	AMControl *pgtIntegrationTime_;
	AMControl *pgtIntegrationMode_;
	AMControl *i0_;
	AMControl *eVFbk_;
	AMControl *photodiode_;
	AMControl *encoderUp_;
	AMControl *encoderDown_;
	AMControl *loadlockCCG_;
	AMControl *loadlockTCG_;
	AMControl *ssaManipulatorX_;
	AMControl *ssaManipulatorXStop_;
	AMControl *ssaManipulatorY_;
	AMControl *ssaManipulatorYStop_;
	AMControl *ssaManipulatorZ_;
	AMControl *ssaManipulatorZStop_;
	AMControl *ssaManipulatorRot_;
	AMControl *ssaManipulatorRotStop_;
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
	AMControl *scalerIntegrationTime_;
	AMControl *scalerScansPerBuffer_;
	AMControl *scalerTotalNumberOfScans_;
	AMControl *scalerMode_;

	AMControlSet *teyControlSet_;
	AMDetector *teyDetector_;
	AMControlSet *tfyControlSet_;
	AMDetector *tfyDetector_;
	AMControlSet *pgtControlSet_;
	AMDetector *pgtDetector_;
	AMDetector *i0Detector_;
	AMControlSet *i0ControlSet_;
	AMDetector *eVFbkDetector_;
	AMControlSet *eVFbkControlSet_;
	AMDetector *photodiodeDetector_;
	AMControlSet *photodiodeControlSet_;
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

	AMDetectorSet *allDetectors_;
	AMDetectorSet *feedbackDetectors_;
	AMDetectorSet *XASDetectors_;

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

	AMBeamlineControlMoveAction *beamOnAction1_;
	AMBeamlineControlMoveAction *beamOnAction2_;
	AMBeamlineParallelActionsList *beamOnActionsList_;

	AMBeamlineControlMoveAction *stopMotorsActions1_;
	AMBeamlineControlMoveAction *stopMotorsActions2_;
	AMBeamlineControlMoveAction *stopMotorsActions3_;
	AMBeamlineControlMoveAction *stopMotorsActions4_;
	AMBeamlineParallelActionsList *stopMotorsActionsList_;

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
