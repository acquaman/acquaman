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

// unused: #include "beamline/AMPVNames.h"
#include "beamline/AMBeamline.h"
#include "beamline/AMDetector.h"
#include "beamline/AMControlSet.h"
#include "util/AMBiHash.h"
#include "beamline/AMBeamlineControlAction.h"
#include "beamline/AMBeamlineControlMoveAction.h"
#include "beamline/AMBeamlineActionsList.h"
#include "beamline/AMBeamlineParallelActionsList.h"

#include "acquaman/AMDetectorInfoList.h"
#include "acquaman/AMControlOptimization.h"


class SGMGratingAction;
class SGMTransferAction1;
class SGMTransferAction2;
class SGMTransferAction3;
class SGMTransferAction4;

class AMSamplePlate;

class SGMBeamline : public AMBeamline
{
	Q_OBJECT

public:
	enum sgmGrating {lowGrating=0, mediumGrating=1, highGrating=2};
	QString sgmGratingName(SGMBeamline::sgmGrating grating) const;
	enum sgmHarmonic {firstHarmonic=1, thirdHarmonic=3};
	QString sgmHarmonicName(SGMBeamline::sgmHarmonic harmonic) const;

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
	AMDetectorInfo* teyDetector() const { return teyDetector_;}
	AMDetectorInfo* tfyDetector() const { return tfyDetector_;}
	AMDetectorInfo* pgtDetector() const { return pgtDetector_;}
	AMDetectorInfo* i0Detector() const { return i0Detector_;}
	AMDetectorInfo* eVFbkDetector() const { return eVFbkDetector_;}
	AMDetectorInfo* photodiodeDetector() const { return photodiodeDetector_;}
	AMDetectorInfo* encoderUpDetector() const { return encoderUpDetector_;}
	AMDetectorInfo* encoderDownDetector() const { return encoderDownDetector_;}
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

	AMDetectorInfoSet* allDetectors() const { return allDetectors_;}
	AMDetectorInfoSet* feedbackDetectors() const { return feedbackDetectors_;}
	AMDetectorInfoSet* XASDetectors() const { return XASDetectors_;}

	AMSamplePlate* currentSamplePlate() const { return currentSamplePlate_; }

	AMBeamlineControlMoveAction* beamOnAction();
	AMBeamlineParallelActionsList* beamOnActionsList();

	AMBeamlineParallelActionsList* stopMotorsActionsList();

	QList<AM1BeamlineActionItem*> transferLoadlockOutActions() const;

	QList<AM1BeamlineActionItem*> transferLoadlockInActions() const;

	QList<AM1BeamlineActionItem*> transferChamberOutActions() const;

	QList<AM1BeamlineActionItem*> transferChamberInActions() const;

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
	void startTransfer() { transferAction1_->start(); }

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
	AMControl *tfyHVSetpoint_;
	AMControl *tfyHVFbk_;
	AMControl *pgt_;
	AMControl *pgtHVSetpoint_;
	AMControl *pgtHVFbk_;
	AMControl *pgtHVRamping_;
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
	AMDetectorInfo *teyDetector_;
	AMControlSet *tfyControlSet_;
	AMDetectorInfo *tfyDetector_;
	AMControlSet *pgtControlSet_;
	AMDetectorInfo *pgtDetector_;
	AMDetectorInfo *i0Detector_;
	AMControlSet *i0ControlSet_;
	AMDetectorInfo *eVFbkDetector_;
	AMControlSet *eVFbkControlSet_;
	AMDetectorInfo *photodiodeDetector_;
	AMControlSet *photodiodeControlSet_;
	AMDetectorInfo *encoderUpDetector_;
	AMControlSet *encoderUpControlSet_;
	AMDetectorInfo *encoderDownDetector_;
	AMControlSet *encoderDownControlSet_;

	AMControlSet* criticalControlsSet_;
	AMControlSet* beamOnControlSet_;

	AMControlOptimization *fluxOptimization_;
	AMControlOptimization *resolutionOptimization_;
	AMControlSet *fluxResolutionSet_;

	AMControlSet *trackingSet_;
	AMControlSet *ssaManipulatorSet_;
	AMDetectorInfoSet *allDetectors_;
	AMDetectorInfoSet *feedbackDetectors_;
	AMDetectorInfoSet *XASDetectors_;

	QList<AMControlSet*> unconnectedSets_;

	/// The sample plate currently in the SSA chamber:
	AMSamplePlate* currentSamplePlate_;

	AM1BeamlineActionItem *transferAction1_;
	AM1BeamlineActionItem *transferAction2_;
	AM1BeamlineActionItem *transferAction3_;
	AM1BeamlineActionItem *transferAction4_;
	AM1BeamlineActionItem *transferAction5_;
	AM1BeamlineActionItem *transferAction6_;
	AM1BeamlineActionItem *transferAction7_;
	AM1BeamlineActionItem *transferAction8_;
	AM1BeamlineActionItem *transferAction9_;
	AM1BeamlineActionItem *transferAction10_;
	AM1BeamlineActionItem *transferAction11_;
	AM1BeamlineActionItem *transferAction12_;
	AM1BeamlineActionItem *transferAction13_;
	AM1BeamlineActionItem *transferAction14_;
	AM1BeamlineActionItem *transferAction15_;
	AM1BeamlineActionItem *transferAction16_;
	AM1BeamlineActionItem *transferAction17_;
	AM1BeamlineActionItem *transferAction18_;
	AM1BeamlineActionItem *transferAction19_;
	AM1BeamlineActionItem *transferAction20_;
	AM1BeamlineActionItem *transferAction21_;
	AM1BeamlineActionItem *transferAction22_;
	AM1BeamlineActionItem *transferAction23_;
	AM1BeamlineActionItem *transferAction24_;
	AM1BeamlineActionItem *transferAction25_;

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



class SGMTransferAction1 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction1(QObject *parent = 0) : AM1BeamlineActionItem("Close the valve between the endstation and the loadlock", parent){
	}
};

class SGMTransferAction2 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction2(QObject *parent = 0) : AM1BeamlineActionItem("Turn off the CCG", parent){
		hasFeedback_ = true;
	}
public slots:
	void checkValue(double value){
		bool rVal = false;
		if(value > 1e-3)
			rVal = true;
		emit ready(rVal);
	}
};

class SGMTransferAction3 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction3(QObject *parent = 0) : AM1BeamlineActionItem("Close the roughing pump valve", parent){
	}
};

class SGMTransferAction4 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction4(QObject *parent = 0) : AM1BeamlineActionItem("Turn off the turbo pump power", parent){
	}
};

class SGMTransferAction5 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction5(QObject *parent = 0) : AM1BeamlineActionItem("Wait for loadlock to reach air pressure", parent){
		hasFeedback_ = true;
	}
public slots:
	void checkValue(double value){
		bool rVal = false;
		if(value > 700)
			rVal = true;
		emit ready(rVal);
	}
};

class SGMTransferAction6 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction6(QObject *parent = 0) : AM1BeamlineActionItem("Open loadlock port", parent){
	}
};

class SGMTransferAction7 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction7(QObject *parent = 0) : AM1BeamlineActionItem("Close loadlock port", parent){
	}
};

class SGMTransferAction8 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction8(QObject *parent = 0) : AM1BeamlineActionItem("Open the roughing pump valve", parent){
	}
};

class SGMTransferAction9 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction9(QObject *parent = 0) : AM1BeamlineActionItem("Wait for loadlock to reach rough vacuum", parent){
		hasFeedback_ = true;
	}
public slots:
	void checkValue(double value){
		bool rVal = false;
		if(value < 200)
			rVal = true;
		emit ready(rVal);
	}
};

class SGMTransferAction10 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction10(QObject *parent = 0) : AM1BeamlineActionItem("Turn on the turbo pump power", parent){
	}
};

class SGMTransferAction11 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction11(QObject *parent = 0) : AM1BeamlineActionItem("Turn on the CCG", parent){
		hasFeedback_ = true;
	}
public slots:
	void checkValue(double value){
		bool rVal = false;
		if(value < 1e-4)
			rVal = true;
		emit ready(rVal);
	}
};

class SGMTransferAction12 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction12(QObject *parent = 0) : AM1BeamlineActionItem("Wait for loadlock to reach high vacuum", parent){
		hasFeedback_ = true;
	}
public slots:
	void checkValue(double value){
		bool rVal = false;
		if(value < 5e-6)
			rVal = true;
		emit ready(rVal);
	}
};

class SGMTransferAction13 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction13(QObject *parent = 0) : AM1BeamlineActionItem("Close Endstation Vacuum Valve", parent){
	}
};

class SGMTransferAction14 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction14(QObject *parent = 0) : AM1BeamlineActionItem("Turn off Detector High Voltage", parent){
	}
};

class SGMTransferAction15 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction15(QObject *parent = 0) : AM1BeamlineActionItem("Retract Detectors", parent){
	}
};

class SGMTransferAction16 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction16(QObject *parent = 0) : AM1BeamlineActionItem("Move to Transfer Position", parent){
	}
};

class SGMTransferAction17 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction17(QObject *parent = 0) : AM1BeamlineActionItem("Confrim Loadlock at High Vacuum", parent){
	}
};

class SGMTransferAction18 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction18(QObject *parent = 0) : AM1BeamlineActionItem("Open Transfer Valve", parent){
	}
};

class SGMTransferAction19 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction19(QObject *parent = 0) : AM1BeamlineActionItem("Transfer Sample Out", parent){
	}
};

class SGMTransferAction20 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction20(QObject *parent = 0) : AM1BeamlineActionItem("Close Transfer Valve", parent){
	}
};

class SGMTransferAction21 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction21(QObject *parent = 0) : AM1BeamlineActionItem("Confirm Loadlock at High Vacuum", parent){
	}
};

class SGMTransferAction22 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction22(QObject *parent = 0) : AM1BeamlineActionItem("Open Transfer Valve", parent){
	}
};

class SGMTransferAction23 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction23(QObject *parent = 0) : AM1BeamlineActionItem("Transfer Sample In", parent){
	}
};

class SGMTransferAction24 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction24(QObject *parent = 0) : AM1BeamlineActionItem("Close Transfer Valve", parent){
	}
};

class SGMTransferAction25 : public AM1BeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction25(QObject *parent = 0) : AM1BeamlineActionItem("Turn on Detector High Voltage", parent){
	}
};


#endif // ACQMAN_SGMBEAMLINE_H
