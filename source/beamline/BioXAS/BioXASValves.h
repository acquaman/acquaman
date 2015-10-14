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
	explicit BioXASValves(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASValves();

	/// Returns true if all subcontrols are presently connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the valves control.
	BioXASValvesControl* valvesControl() const { return valvesControl_; }

	/// Returns the set of front end vacuum valves.
	AMControlSet* frontEndValves() const { return frontEndValves_; }
	/// Returns the first front end vacuum valve control.
	CLSBiStateControl *vvr1() const { return vvr1_; }
	/// Returns the second front end vacuum valve control.
	CLSBiStateControl *vvr2() const { return vvr2_; }
	/// Returns the third front end vacuum valve control.
	CLSBiStateControl *vvr3() const { return vvr3_; }
	/// Returns the fourth front end vacuum valve control.
	CLSBiStateControl *vvr4() const { return vvr4_; }

	/// Returns the set of Side beamline vacuum valves.
	AMControlSet* sideValves() const { return sideValves_; }
	/// The first Side beamline vacuum valve.
	CLSBiStateControl *vvrSide1() const { return vvrSide1_; }
	/// The second Side beamline vacuum valve.
	CLSBiStateControl *vvrSide2() const { return vvrSide2_; }
	/// The third Side beamline vacuum valve.
	CLSBiStateControl *vvrSide3() const { return vvrSide3_; }
	/// The fourth Side beamline vacuum valve.
	CLSBiStateControl *vvrSide4() const { return vvrSide4_; }
	/// The fifth Side beamline vacuum valve.
	CLSBiStateControl *vvrSide5() const { return vvrSide5_; }

	/// Returns the set of Main beamline vacuum valves.
	AMControlSet* mainValves() const { return mainValves_; }
	/// The first Main beamline vacuum valve.
	CLSBiStateControl *vvrMain1() const { return vvrMain1_; }
	/// The second Main beamline vacuum valve.
	CLSBiStateControl *vvrMain2() const { return vvrMain2_; }
	/// The third Main beamline vacuum valve.
	CLSBiStateControl *vvrMain3() const { return vvrMain3_; }
	/// The fourth Main beamline vacuum valve.
	CLSBiStateControl *vvrMain4() const { return vvrMain4_; }
	/// The fifth Main beamline vacuum valve.
	CLSBiStateControl *vvrMain5() const { return vvrMain5_; }

	/// Returns the set of Imaging beamline vacuum valves.
	AMControlSet* imagingValves() const { return imagingValves_; }
	/// The first Imaging beamline vacuum valve.
	CLSBiStateControl *vvrImaging1() const { return vvrImaging1_; }
	/// The second Imaging beamline vacuum valve.
	CLSBiStateControl *vvrImaging2() const { return vvrImaging2_; }
	/// The third Imaging beamline vacuum valve.
	CLSBiStateControl *vvrImaging3() const { return vvrImaging3_; }
	/// The fourth Imaging beamline vacuum valve.
	CLSBiStateControl *vvrImaging4() const { return vvrImaging4_; }
	/// The fifth Imaging beamline vacuum valve.
	CLSBiStateControl *vvrImaging5() const { return vvrImaging5_; }
	/// The sixth Imaging beamline vacuum valve.
	CLSBiStateControl *vvrImaging6() const { return vvrImaging6_; }
	/// The seventh Imaging beamline vacuum valve.
	CLSBiStateControl *vvrImaging7() const { return vvrImaging7_; }
	/// The eighth Imaging beamline vacuum valve.
	CLSBiStateControl *vvrImaging8() const { return vvrImaging8_; }

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
	/// The first Side beamline vacuum valve.
	CLSBiStateControl *vvrSide1_;
	/// The second Side beamline vacuum valve.
	CLSBiStateControl *vvrSide2_;
	/// The third Side beamline vacuum valve.
	CLSBiStateControl *vvrSide3_;
	/// The fourth Side beamline vacuum valve.
	CLSBiStateControl *vvrSide4_;
	/// The fifth Side beamline vacuum valve.
	CLSBiStateControl *vvrSide5_;

	/// The set of Main beamline vacuum valves.
	AMControlSet *mainValves_;
	/// The first Main beamline vacuum valve.
	CLSBiStateControl *vvrMain1_;
	/// The second Main beamline vacuum valve.
	CLSBiStateControl *vvrMain2_;
	/// The third Main beamline vacuum valve.
	CLSBiStateControl *vvrMain3_;
	/// The fourth Main beamline vacuum valve.
	CLSBiStateControl *vvrMain4_;
	/// The fifth Main beamline vacuum valve.
	CLSBiStateControl *vvrMain5_;

	/// The set of Imaging beamline vacuum valves.
	AMControlSet *imagingValves_;
	/// The first Imaging beamline vacuum valve.
	CLSBiStateControl *vvrImaging1_;
	/// The second Imaging beamline vacuum valve.
	CLSBiStateControl *vvrImaging2_;
	/// The third Imaging beamline vacuum valve.
	CLSBiStateControl *vvrImaging3_;
	/// The fourth Imaging beamline vacuum valve.
	CLSBiStateControl *vvrImaging4_;
	/// The fifth Imaging beamline vacuum valve.
	CLSBiStateControl *vvrImaging5_;
	/// The sixth Imaging beamline vacuum valve.
	CLSBiStateControl *vvrImaging6_;
	/// The seventh Imaging beamline vacuum valve.
	CLSBiStateControl *vvrImaging7_;
	/// The eighth Imaging beamline vacuum valve.
	CLSBiStateControl *vvrImaging8_;

	/// The valves control.
	BioXASValvesControl *valvesControl_;
};

#endif // BIOXASVALVES_H
