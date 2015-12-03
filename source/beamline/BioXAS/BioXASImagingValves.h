#ifndef BIOXASIMAGINGVALVES_H
#define BIOXASIMAGINGVALVES_H

#include "beamline/BioXAS/BioXASValvesSetControl.h"

class BioXASImagingValves : public BioXASValvesSetControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASImagingValves(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASImagingValves();

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

protected:
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
};

#endif // BIOXASIMAGINGVALVES_H
