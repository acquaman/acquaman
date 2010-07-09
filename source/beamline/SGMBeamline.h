#ifndef ACQMAN_SGMBEAMLINE_H
#define ACQMAN_SGMBEAMLINE_H

#include "AMPVNames.h"
#include "AMDetector.h"
#include "AMControlSet.h"
#include "AMBiHash.h"
#include "AMBeamlineControlAction.h"

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
	AMAbstractDetector* teyDetector() const { return teyDetector_;}
	AMAbstractDetector* tfyDetector() const { return tfyDetector_;}
	AMAbstractDetector* pgtDetector() const { return pgtDetector_;}
	AMAbstractDetector* i0Detector() const { return i0Detector_;}
	AMAbstractDetector* eVFbkDetector() const { return eVFbkDetector_;}
	AMControl* loadlockPressure() const { return loadlockPressure_;}

	AMControlSet* fluxResolutionSet() const { return fluxResolutionSet_;}
	AMControlSet* trackingSet() const { return trackingSet_;}
	AMAbstractDetectorSet* allDetectors() const { return allDetectors_;}
	AMAbstractDetectorSet* XASDetectors() const { return XASDetectors_;}

	QList<AMBeamlineActionItem*> transferActions() const {
		QList<AMBeamlineActionItem*> rVal;
		rVal.append(transferAction1_);
		rVal.append(transferAction2_);
		rVal.append(transferAction3_);
		rVal.append(transferAction4_);
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
	AMControl *pgt_;
	AMControl *i0_;
	AMControl *eVFbk_;
	AMControl *loadlockPressure_;

	AMAbstractDetector *teyDetector_;
	AMAbstractDetector *tfyDetector_;
	AMAbstractDetector *pgtDetector_;
	AMAbstractDetector *i0Detector_;
	AMAbstractDetector *eVFbkDetector_;

	AMControlOptimization *fluxOptimization_;
	AMControlOptimization *resolutionOptimization_;
	AMControlSet *fluxResolutionSet_;

	AMControlSet *trackingSet_;
	AMAbstractDetectorSet *allDetectors_;
	AMAbstractDetectorSet *XASDetectors_;

	AMBeamlineActionItem *transferAction1_;
	AMBeamlineActionItem *transferAction2_;
	AMBeamlineActionItem *transferAction3_;
	AMBeamlineActionItem *transferAction4_;

	AMBiHash<QString, QString> amNames2pvNames_;

	friend class SGMGratingAction;
};

class SGMFluxOptimization : public AMControlOptimization
{
	Q_OBJECT
public:
	SGMFluxOptimization(QObject *parent=0);

	virtual QMap<double, double> curve(QList<QVariant> stateParameters, AMRegionsList* contextParameters);


protected:
	double maximumEnergy(AMRegionsList* regions);
	double minimumEnergy(AMRegionsList* regions);
};

class SGMResolutionOptimization : public AMControlOptimization
{
	Q_OBJECT
public:
	SGMResolutionOptimization(QObject *parent=0);

	QMap<double, double> curve(QList<QVariant> stateParameters, AMRegionsList* contextParameters);

protected:
	double maximumEnergy(AMRegionsList* regions);
	double minimumEnergy(AMRegionsList* regions);
};



class SGMTransferAction1 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction1(QObject *parent = 0) : AMBeamlineActionItem(parent){;}

public slots:
	void start(){
		qDebug() << "First";
		emit ready(true);
	}
};

class SGMTransferAction2 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction2(QObject *parent = 0) : AMBeamlineActionItem(parent){;}

public slots:
	void start(){
		qDebug() << "Second";
		emit ready(true);
	}

	void checkValue(double value){
		if(value > 1e-3)
			emit succeeded();
	}
};

class SGMTransferAction3 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction3(QObject *parent = 0) : AMBeamlineActionItem(parent){;}

public slots:
	void start(){
		qDebug() << "Third";
		emit ready(true);
	}
};

class SGMTransferAction4 : public AMBeamlineActionItem
{
	Q_OBJECT
public:
	SGMTransferAction4(QObject *parent = 0) : AMBeamlineActionItem(parent){;}

public slots:
	void start(){
		qDebug() << "Fourth";
		emit ready(true);
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
