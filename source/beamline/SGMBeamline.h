#ifndef SGMBEAMLINE_H
#define SGMBEAMLINE_H

#include "Control.h"
#include "PVNames.h"

class SGMBeamline : public Control
{
    Q_OBJECT

public:
    static SGMBeamline* sgm();		// singleton-class accessor
    static void releaseSGM();	// releases memory for Beamline

    virtual ~SGMBeamline();

    // What does this Beamline have? (These objects will be useful in the scripting world too!)
    ///////////////////////////////////

    ReadOnlyPVControl* ringCurrent() const { return ringCurrent_; }
    PVControl* energy() const { return energy_;}
    PVControl* exitSlitGap() const { return exitSlitGap_;}

protected:
    // Singleton implementation:
    SGMBeamline();					// protected constructor... only access through Beamline::bl()
    static SGMBeamline* instance_;

    // Parts of this beamline:
    ///////////////////////////////

    ReadOnlyPVControl* ringCurrent_;
    PVControl *energy_;
    PVControl *exitSlitGap_;
};

#endif // SGMBEAMLINE_H
