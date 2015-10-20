#ifndef BIOXASVALVES_H
#define BIOXASVALVES_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/BioXAS/BioXASValvesControl.h"
#include "beamline/CLS/CLSBiStateControl.h"

class BioXASValves : public BioXASValvesControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASValves(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASValves();

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
};

#endif // BIOXASVALVES_H
