#ifndef BIOXASMAINVALVES_H
#define BIOXASMAINVALVES_H

#include "beamline/BioXAS/BioXASValvesSetControl.h"

class BioXASMainValves : public BioXASValvesSetControl
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASMainValves(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMainValves();

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

protected:
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
};

#endif // BIOXASMAINVALVES_H
