#ifndef ACQMAN_SGMBEAMLINE_H
#define ACQMAN_SGMBEAMLINE_H

//#include "AMControl.h"
#include "AMPVNames.h"
#include "AMDetector.h"
#include "AMControlSet.h"
#include "AMBiHash.h"


class SGMBeamline : public AMControl
{
	Q_OBJECT

public:
	enum sgmGrating {lowGrating=0, mediumGrating=1, highGrating=2};
	enum sgmHarmonic {firstHarmonic=1, thirdHarmonic=3};

	static SGMBeamline* sgm();		// singleton-class accessor
	static void releaseSGM();	// releases memory for Beamline

	virtual ~SGMBeamline();

	// What does this Beamline have? (These objects will be useful in the scripting world too!)
	///////////////////////////////////

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
	AMControl* harmonic() const { return harmonic_;}
	AMControl* undulatorTracking() const { return undulatorTracking_;}
	AMControl* monoTracking() const { return monoTracking_;}
	AMControl* exitSlitTracking() const { return exitSlitTracking_;}
	AMDetector* teyDetector() const { return teyDetector_;}
	AMDetector* tfyDetector() const { return tfyDetector_;}
	AMDetector* pgtDetector() const { return pgtDetector_;}

	AMControlSet* fluxResolutionSet() const { return fluxResolutionSet_;}
	AMControlSet* trackingSet() const { return trackingSet_;}
	AMDetectorSet* XASDetectors() const { return XASDetectors_;}

	bool energyValidForSettings(sgmGrating grating, sgmHarmonic harmonic, double energy);
	bool energyRangeValidForSettings(sgmGrating grating, sgmHarmonic harmonic, double minEnergy, double maxEnergy);

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
	AMControl *harmonic_;
	AMControl *undulatorTracking_;
	AMControl *monoTracking_;
	AMControl *exitSlitTracking_;
	AMControl *tey_;
	AMControl *tfy_;
	AMControl *pgt_;

	AMDetector *teyDetector_;
	AMDetector *tfyDetector_;
	AMDetector *pgtDetector_;

	AMControlOptimization *fluxOptimization_;
	AMControlOptimization *resolutionOptimization_;
	AMControlSet *fluxResolutionSet_;

	AMControlSet *trackingSet_;
	AMDetectorSet *XASDetectors_;

	AMBiHash<QString, QString> amNames2pvNames_;
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

#endif // ACQMAN_SGMBEAMLINE_H
