#ifndef ACQMAN_SGMBEAMLINE_H
#define ACQMAN_SGMBEAMLINE_H

#include "AMPVNames.h"
#include "AMDetector.h"
#include "AMControlSet.h"
#include "AMBiHash.h"
#include "AMBeamlineControlAction.h"

#include <gsl/gsl_multifit.h>

class SGMGratingAction;
class SGMTransferAction1;
class SGMTransferAction2;
class SGMTransferAction3;
class SGMTransferAction4;

class SGMBeamline : public AMControl
{
	Q_OBJECT

public:
	enum sgmGrating {lowGrating=0, mediumGrating=1, highGrating=2};
	QString sgmGratingName(SGMBeamline::sgmGrating grating) const {
		if(grating == 0)
			return "lowGrating";
		else if(grating == 1)
			return "mediumGrating";
		else if(grating == 2)
			return "highGrating";
	}
	enum sgmHarmonic {firstHarmonic=1, thirdHarmonic=3};
	QString sgmHarmonicName(SGMBeamline::sgmHarmonic harmonic) const {
		if(harmonic == 1)
			return "firstHarmonic";
		else if(harmonic == 3)
			return "thirdHarmonic";
	}

	static SGMBeamline* sgm();		// singleton-class accessor
	static void releaseSGM();	// releases memory for Beamline

	virtual ~SGMBeamline();

	bool isConnected() const {
		for(int x = 0; x < numChildren(); x++)
			if(!children_.at(x)->isConnected())
				return false;
		return true;
	}

	QString pvName(const QString &amName) const { return amNames2pvNames_.valueF(amName);}
	QString amName(const QString &pvName) const { return amNames2pvNames_.valueR(pvName);}

	AMControl* ringCurrent() const { return ringCurrent_; }
	AMControl* energy() const { return energy_;}
	AMControl* exitSlitGap() const { return exitSlitGap_;}
	AMControl* m4() const { return m4_;}
	AMControl* grating() const { return grating_;}
	AMBeamlineActionItem *gratingAction() const { return gratingAction_;}
	AMControl* harmonic() const { return harmonic_;}
	AMControl* undulatorTracking() const { return undulatorTracking_;}
	AMControl* monoTracking() const { return monoTracking_;}
	AMControl* exitSlitTracking() const { return exitSlitTracking_;}
	AMDetectorInfo* teyDetector() const { return teyDetector_;}
	AMDetectorInfo* tfyDetector() const { return tfyDetector_;}
	AMDetectorInfo* pgtDetector() const { return pgtDetector_;}
	AMDetectorInfo* i0Detector() const { return i0Detector_;}
	AMDetectorInfo* eVFbkDetector() const { return eVFbkDetector_;}
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

	AMControlSet* fluxResolutionSet() const { return fluxResolutionSet_;}
	AMControlSet* trackingSet() const { return trackingSet_;}
	AMControlSet* ssaManipulatorSet() const { return ssaManipulatorSet_; }
	AMDetectorInfoSet* allDetectors() const { return allDetectors_;}
	AMDetectorInfoSet* XASDetectors() const { return XASDetectors_;}

	QList<AMBeamlineActionItem*> transferLoadlockOutActions() const {
		QList<AMBeamlineActionItem*> rVal;
		rVal.append(transferAction1_);
		rVal.append(transferAction2_);
		rVal.append(transferAction3_);
		rVal.append(transferAction4_);
		rVal.append(transferAction5_);
		rVal.append(transferAction6_);
		return rVal;
	}

	QList<AMBeamlineActionItem*> transferLoadlockInActions() const {
		QList<AMBeamlineActionItem*> rVal;
		rVal.append(transferAction7_);
		rVal.append(transferAction8_);
		rVal.append(transferAction9_);
		rVal.append(transferAction10_);
		rVal.append(transferAction11_);
		rVal.append(transferAction12_);
		return rVal;
	}

	QList<AMBeamlineActionItem*> transferChamberOutActions() const {
		QList<AMBeamlineActionItem*> rVal;
		rVal.append(transferAction13_);
		rVal.append(transferAction14_);
		rVal.append(transferAction15_);
		rVal.append(transferAction16_);
		rVal.append(transferAction17_);
		rVal.append(transferAction18_);
		rVal.append(transferAction19_);
		rVal.append(transferAction20_);
		return rVal;
	}

	QList<AMBeamlineActionItem*> transferChamberInActions() const {
		QList<AMBeamlineActionItem*> rVal;
		rVal.append(transferAction21_);
		rVal.append(transferAction22_);
		rVal.append(transferAction23_);
		rVal.append(transferAction24_);
		rVal.append(transferAction25_);
		return rVal;
	}

	bool energyValidForSettings(sgmGrating grating, sgmHarmonic harmonic, double energy);
	bool energyRangeValidForSettings(sgmGrating grating, sgmHarmonic harmonic, double minEnergy, double maxEnergy);

	QList< QPair<sgmGrating, sgmHarmonic> > gratingHarmonicForEnergyRange(double minEnergy, double maxEnergy);
	QPair<double, double> energyRangeForGratingHarmonic(sgmGrating grating, sgmHarmonic harmonic);

public slots:
	void startTransfer() { transferAction1_->start(); }

protected:
	// Singleton implementation:
	SGMBeamline();					// protected constructor... only access through Beamline::bl()
	static SGMBeamline* instance_;

	// Parts of this beamline:
	///////////////////////////////

	AMControl *ringCurrent_;
	AMControl *energy_;
	AMControl *exitSlitGap_;
	AMControl *m4_;
	AMControl *grating_;
	AMBeamlineActionItem *gratingAction_;
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

	AMDetectorInfo *teyDetector_;
	AMDetectorInfo *tfyDetector_;
	AMDetectorInfo *pgtDetector_;
	AMDetectorInfo *i0Detector_;
	AMDetectorInfo *eVFbkDetector_;

	AMControlOptimization *fluxOptimization_;
	AMControlOptimization *resolutionOptimization_;
	AMControlSet *fluxResolutionSet_;

	AMControlSet *trackingSet_;
	AMControlSet *ssaManipulatorSet_;
	AMDetectorInfoSet *allDetectors_;
	AMDetectorInfoSet *XASDetectors_;

	AMBeamlineActionItem *transferAction1_;
	AMBeamlineActionItem *transferAction2_;
	AMBeamlineActionItem *transferAction3_;
	AMBeamlineActionItem *transferAction4_;
	AMBeamlineActionItem *transferAction5_;
	AMBeamlineActionItem *transferAction6_;
	AMBeamlineActionItem *transferAction7_;
	AMBeamlineActionItem *transferAction8_;
	AMBeamlineActionItem *transferAction9_;
	AMBeamlineActionItem *transferAction10_;
	AMBeamlineActionItem *transferAction11_;
	AMBeamlineActionItem *transferAction12_;
	AMBeamlineActionItem *transferAction13_;
	AMBeamlineActionItem *transferAction14_;
	AMBeamlineActionItem *transferAction15_;
	AMBeamlineActionItem *transferAction16_;
	AMBeamlineActionItem *transferAction17_;
	AMBeamlineActionItem *transferAction18_;
	AMBeamlineActionItem *transferAction19_;
	AMBeamlineActionItem *transferAction20_;
	AMBeamlineActionItem *transferAction21_;
	AMBeamlineActionItem *transferAction22_;
	AMBeamlineActionItem *transferAction23_;
	AMBeamlineActionItem *transferAction24_;
	AMBeamlineActionItem *transferAction25_;


	AMBiHash<QString, QString> amNames2pvNames_;

	friend class SGMGratingAction;
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



class SGMTransferAction1 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction1(QObject *parent = 0) : AMBeamlineActionItem("Close the valve between the endstation and the loadlock", parent){
	}
};

class SGMTransferAction2 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction2(QObject *parent = 0) : AMBeamlineActionItem("Turn off the CCG", parent){
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

class SGMTransferAction3 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction3(QObject *parent = 0) : AMBeamlineActionItem("Close the roughing pump valve", parent){
	}
};

class SGMTransferAction4 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction4(QObject *parent = 0) : AMBeamlineActionItem("Turn off the turbo pump power", parent){
	}
};

class SGMTransferAction5 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction5(QObject *parent = 0) : AMBeamlineActionItem("Wait for loadlock to reach air pressure", parent){
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

class SGMTransferAction6 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction6(QObject *parent = 0) : AMBeamlineActionItem("Open loadlock port", parent){
	}
};

class SGMTransferAction7 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction7(QObject *parent = 0) : AMBeamlineActionItem("Close loadlock port", parent){
	}
};

class SGMTransferAction8 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction8(QObject *parent = 0) : AMBeamlineActionItem("Open the roughing pump valve", parent){
	}
};

class SGMTransferAction9 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction9(QObject *parent = 0) : AMBeamlineActionItem("Wait for loadlock to reach rough vacuum", parent){
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

class SGMTransferAction10 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction10(QObject *parent = 0) : AMBeamlineActionItem("Turn on the turbo pump power", parent){
	}
};

class SGMTransferAction11 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction11(QObject *parent = 0) : AMBeamlineActionItem("Turn on the CCG", parent){
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

class SGMTransferAction12 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction12(QObject *parent = 0) : AMBeamlineActionItem("Wait for loadlock to reach high vacuum", parent){
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

class SGMTransferAction13 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction13(QObject *parent = 0) : AMBeamlineActionItem("Close Endstation Vacuum Valve", parent){
	}
};

class SGMTransferAction14 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction14(QObject *parent = 0) : AMBeamlineActionItem("Turn off Detector High Voltage", parent){
	}
};

class SGMTransferAction15 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction15(QObject *parent = 0) : AMBeamlineActionItem("Retract Detectors", parent){
	}
};

class SGMTransferAction16 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction16(QObject *parent = 0) : AMBeamlineActionItem("Move to Transfer Position", parent){
	}
};

class SGMTransferAction17 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction17(QObject *parent = 0) : AMBeamlineActionItem("Confrim Loadlock at High Vacuum", parent){
	}
};

class SGMTransferAction18 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction18(QObject *parent = 0) : AMBeamlineActionItem("Open Transfer Valve", parent){
	}
};

class SGMTransferAction19 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction19(QObject *parent = 0) : AMBeamlineActionItem("Transfer Sample Out", parent){
	}
};

class SGMTransferAction20 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction20(QObject *parent = 0) : AMBeamlineActionItem("Close Transfer Valve", parent){
	}
};

class SGMTransferAction21 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction21(QObject *parent = 0) : AMBeamlineActionItem("Confirm Loadlock at High Vacuum", parent){
	}
};

class SGMTransferAction22 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction22(QObject *parent = 0) : AMBeamlineActionItem("Open Transfer Valve", parent){
	}
};

class SGMTransferAction23 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction23(QObject *parent = 0) : AMBeamlineActionItem("Transfer Sample In", parent){
	}
};

class SGMTransferAction24 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction24(QObject *parent = 0) : AMBeamlineActionItem("Close Transfer Valve", parent){
	}
};

class SGMTransferAction25 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction25(QObject *parent = 0) : AMBeamlineActionItem("Turn on Detector High Voltage", parent){
	}
};

class SGMGratingAction : public AMBeamlineControlAction
{
	Q_OBJECT
public:
	SGMGratingAction(AMControl *grating, QObject *parent = 0);

public slots:
	void start();
};


#endif // ACQMAN_SGMBEAMLINE_H
