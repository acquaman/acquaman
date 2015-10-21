#ifndef BIOXASSIDEVALVES_H
#define BIOXASSIDEVALVES_H

#include "beamline/BioXAS/BioXASValvesSetControl.h"

class BioXASSideValves : public BioXASValvesSetControl
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASSideValves(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideValves();

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
};

#endif // BIOXASSIDEVALVES_H
