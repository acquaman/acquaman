#ifndef ACQMAN_SGMBEAMLINE_H
#define ACQMAN_SGMBEAMLINE_H

#include "AMControl.h"
#include "AMPVNames.h"

class AMSGMBeamline : public AMControl
{
    Q_OBJECT

public:
    enum sgmGrating {lowGrating=0, mediumGrating=1, highGrating=2};

    static AMSGMBeamline* sgm();		// singleton-class accessor
    static void releaseSGM();	// releases memory for Beamline

    virtual ~AMSGMBeamline();

    // What does this Beamline have? (These objects will be useful in the scripting world too!)
    ///////////////////////////////////

	AMControl* ringCurrent() const { return ringCurrent_; }
	AMControl* energy() const { return energy_;}
	AMControl* exitSlitGap() const { return exitSlitGap_;}
	AMControl* m4() const { return m4_;}
	AMControl* grating() const { return grating_;}
	AMControl* undulatorTracking() const { return undulatorTracking_;}
	AMControl* monoTracking() const { return monoTracking_;}
	AMControl* exitSlitTracking() const { return exitSlitTracking_;}

protected:
    // Singleton implementation:
    AMSGMBeamline();					// protected constructor... only access through Beamline::bl()
    static AMSGMBeamline* instance_;

    // Parts of this beamline:
    ///////////////////////////////

	AMControl* ringCurrent_;
	AMControl *energy_;
	AMControl *exitSlitGap_;
	AMControl* m4_;
	AMControl *grating_;
	AMControl *undulatorTracking_;
	AMControl *monoTracking_;
	AMControl *exitSlitTracking_;
};

#endif // SGMBEAMLINE_H
