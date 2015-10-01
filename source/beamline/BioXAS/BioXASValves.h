#ifndef BIOXASVALVES_H
#define BIOXASVALVES_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/BioXAS/BioXASValvesControl.h"
#include "beamline/CLS/CLSBiStateControl.h"

class BioXASValves : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASValves(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASValves();

	/// Returns the valves control.
	AMControl* valvesControl() const { return valvesControl_; }

	/// Returns the set of front end vacuum valves.
	AMControlSet* frontEndValves() const { return frontEndValves_; }
	/// Returns the first front end vacuum valve control.
	AMControl *vvr1() const { return vvr1_; }
	/// Returns the second front end vacuum valve control.
	AMControl *vvr2() const { return vvr2_; }
	/// Returns the third front end vacuum valve control.
	AMControl *vvr3() const { return vvr3_; }
	/// Returns the fourth front end vacuum valve control.
	AMControl *vvr4() const { return vvr4_; }
	/// Returns the fifth front end vacuum valve control.
	AMControl *vvr5() const { return vvr5_; }

	/// Returns the set of Side beamline vacuum valves.
	AMControlSet* sideValves() const { return sideValves_; }

	AMControl *vvrSide1() const { return vvrSide1_; }
	AMControl *vvrSide2() const { return vvrSide2_; }
	AMControl *vvrSide3() const { return vvrSide3_; }
	AMControl *vvrSide4() const { return vvrSide4_; }
	AMControl *vvrSide5() const { return vvrSide5_; }
	AMControl *vvrSide6() const { return vvrSide6_; }

signals:

public slots:

protected:
	/// The set of front end vacuum valves.
	AMControlSet *frontEndValves_;
	/// The first front end vacuum valve control.
	CLSBiStateControl *vvr1_;
	/// The second front end vacuum valve control.
	CLSBiStateControl *vvr2_;
	/// The third front end vacuum valve control.
	CLSBiStateControl *vvr3_;
	/// The fourth front end vacuum valve control.
	CLSBiStateControl *vvr4_;
	/// The fifth front end vacuum valve control.
	CLSBiStateControl *vvr5_;

	/// The set of Side beamline vacuum valves.
	AMControlSet *sideValves_;
	/// The Side beamline vacuum valve controls.
	CLSBiStateControl *vvrSide1_;
	CLSBiStateControl *vvrSide2_;
	CLSBiStateControl *vvrSide3_;
	CLSBiStateControl *vvrSide4_;
	CLSBiStateControl *vvrSide5_;
	CLSBiStateControl *vvrSide6_;

	/// The set of Main beamline vacuum valves.
	AMControlSet *mainValves_;
	/// The Main beamline vacuum valve controls.
	CLSBiStateControl *vvrMain1_;
	CLSBiStateControl *vvrMain2_;
	CLSBiStateControl *vvrMain3_;
	CLSBiStateControl *vvrMain4_;
	CLSBiStateControl *vvrMain5_;
	CLSBiStateControl *vvrMain6_;

	/// The valves control.
	BioXASValvesControl *valvesControl_;
};

#endif // BIOXASVALVES_H
