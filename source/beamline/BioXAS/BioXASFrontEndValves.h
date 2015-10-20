#ifndef BIOXASFRONTENDVALVES_H
#define BIOXASFRONTENDVALVES_H

#include "beamline/BioXAS/BioXASValvesSetControl.h"

class BioXASFrontEndValves : public BioXASValvesSetControl
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASFrontEndValves(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFrontEndValves();

	/// Returns the first front end vacuum valve control.
	AMReadOnlyPVControl *vvr1() const { return vvr1_; }
	/// Returns the second front end vacuum valve control.
	AMReadOnlyPVControl *vvr2() const { return vvr2_; }
	/// Returns the third front end vacuum valve control.
	CLSBiStateControl *vvr3() const { return vvr3_; }
	/// Returns the fourth front end vacuum valve control.
	CLSBiStateControl *vvr4() const { return vvr4_; }
	/// Returns the fifth front end vacuum valve control.
	CLSBiStateControl *vvr5() const { return vvr5_; }
	/// Returns the sixth front end vacuum valve control.
	CLSBiStateControl *vvr6() const { return vvr6_; }

protected:
	/// The first front end vacuum valve control.
	AMReadOnlyPVControl *vvr1_;
	/// The second front end vacuum valve control.
	AMReadOnlyPVControl *vvr2_;
	/// The third front end vacuum valve control.
	CLSBiStateControl *vvr3_;
	/// The fourth front end vacuum valve control.
	CLSBiStateControl *vvr4_;
	/// The fifth front end vacuum valve control.
	CLSBiStateControl *vvr5_;
	/// The sixth front end vacuum valve control.
	CLSBiStateControl *vvr6_;
};

#endif // BIOXASFRONTENDVALVES_H
