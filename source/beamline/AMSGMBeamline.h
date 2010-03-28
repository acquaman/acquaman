#ifndef ACQMAN_SGMBEAMLINE_H
#define ACQMAN_SGMBEAMLINE_H

//#include "AMControl.h"
#include "AMPVNames.h"
#include "AMControlSet.h"

class AMSGMBeamline : public AMControl
{
    Q_OBJECT

public:
    enum sgmGrating {lowGrating=0, mediumGrating=1, highGrating=2};
    enum sgmHarmonic {firstHarmonic=1, thirdHarmonic=3};

    static AMSGMBeamline* sgm();		// singleton-class accessor
    static void releaseSGM();	// releases memory for Beamline

    virtual ~AMSGMBeamline();

    // What does this Beamline have? (These objects will be useful in the scripting world too!)
    ///////////////////////////////////

    AMReadOnlyPVControl* ringCurrent() const { return ringCurrent_; }
    AMPVControl* energy() const { return energy_;}
    AMPVControl* exitSlitGap() const { return exitSlitGap_;}
    AMReadOnlyPVControl* m4() const { return m4_;}
    AMPVControl* grating() const { return grating_;}
    AMPVControl* harmonic() const { return harmonic_;}
    AMPVControl* undulatorTracking() const { return undulatorTracking_;}
    AMPVControl* monoTracking() const { return monoTracking_;}
    AMPVControl* exitSlitTracking() const { return exitSlitTracking_;}

    bool energyValidForSettings(sgmGrating grating, sgmHarmonic harmonic, double energy);
    bool energyRangeValidForSettings(sgmGrating grating, sgmHarmonic harmonic, double minEnergy, double maxEnergy);

protected:
    // Singleton implementation:
    AMSGMBeamline();					// protected constructor... only access through Beamline::bl()
    static AMSGMBeamline* instance_;

    // Parts of this beamline:
    ///////////////////////////////

    AMReadOnlyPVControl* ringCurrent_;
    AMPVControl *energy_;
    AMPVControl *exitSlitGap_;
    AMReadOnlyPVControl* m4_;
    AMPVControl *grating_;
    AMPVControl *harmonic_;
    AMPVControl *undulatorTracking_;
    AMPVControl *monoTracking_;
    AMPVControl *exitSlitTracking_;
};

class AMSGMFluxOptimization : public AMControlOptimization
{
    Q_OBJECT
public:
    AMSGMFluxOptimization(QObject *parent=0);

    QMap<double, double> curve(QList<QVariant> stateParameters, QList<AMXASRegion*> contextParameters);

protected:
    double maximumEnergy(QList<AMXASRegion*> regions);
    double minimumEnergy(QList<AMXASRegion*> regions);
};

#endif // SGMBEAMLINE_H
