#ifndef BIOXASSIDEBEAMLINEUTILITIES_H
#define BIOXASSIDEBEAMLINEUTILITIES_H

#include "beamline/BioXAS/BioXASBeamlineUtilities.h"

class BioXASSideBeamlineUtilities : public BioXASBeamlineUtilities
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASSideBeamlineUtilities(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideBeamlineUtilities();

	// Pressure monitors.

	AMControl *ccgSide1() const { return ccgSide1_; }
	AMControl *ccgSide2() const { return ccgSide2_; }
	AMControl *ccgSide3() const { return ccgSide3_; }
	AMControl *ccgSide4() const { return ccgSide4_; }
	AMControl *ccgSide5() const { return ccgSide5_; }
	AMControl *ccgSide6() const { return ccgSide6_; }

	// Vaccum values.


	AMControl *vvrSide1() const { return vvrSide1_; }
	AMControl *vvrSide2() const { return vvrSide2_; }
	AMControl *vvrSide3() const { return vvrSide3_; }
	AMControl *vvrSide4() const { return vvrSide4_; }
	AMControl *vvrSide5() const { return vvrSide5_; }
	AMControl *vvrSide6() const { return vvrSide6_; }

	// Ion pumps.


	AMControl *iopSide1() const { return iopSide1_; }
	AMControl *iopSide2() const { return iopSide2_; }
	AMControl *iopSide3() const { return iopSide3_; }
	AMControl *iopSide4() const { return iopSide4_; }
	AMControl *iopSide5() const { return iopSide5_; }
	AMControl *iopSide6() const { return iopSide6_; }
	AMControl *iopSide7() const { return iopSide7_; }

	// Flow transducers.

	AMControl *flt5() const { return flt5_; }
	AMControl *flt6() const { return flt6_; }
	AMControl *flt7() const { return flt7_; }
	AMControl *flt8() const { return flt8_; }
	AMControl *flt9() const { return flt9_; }
	AMControl *flt10() const { return flt10_; }
	AMControl *flt11() const { return flt11_; }
	AMControl *flt12() const { return flt12_; }
	AMControl *flt13() const { return flt13_; }
	AMControl *flt14() const { return flt14_; }
	AMControl *flt15() const { return flt15_; }
	AMControl *flt16() const { return flt16_; }
	AMControl *flt17() const { return flt17_; }
	AMControl *flt18() const { return flt18_; }
	AMControl *flt19() const { return flt19_; }
	AMControl *flt20() const { return flt20_; }

	// Flow switches.



	// Temperature monitors.

protected:
	// Pressure controls

	AMReadOnlyPVwStatusControl *ccgSide1_;
	AMReadOnlyPVwStatusControl *ccgSide2_;
	AMReadOnlyPVwStatusControl *ccgSide3_;
	AMReadOnlyPVwStatusControl *ccgSide4_;
	AMReadOnlyPVwStatusControl *ccgSide5_;
	AMReadOnlyPVwStatusControl *ccgSide6_;

	// Vacuum valve controls

	CLSBiStateControl *vvrSide1_;
	CLSBiStateControl *vvrSide2_;
	CLSBiStateControl *vvrSide3_;
	CLSBiStateControl *vvrSide4_;
	CLSBiStateControl *vvrSide5_;
	CLSBiStateControl *vvrSide6_;

	// Ion pump controls

	AMReadOnlyPVControl *iopSide1_;
	AMReadOnlyPVControl *iopSide2_;
	AMReadOnlyPVControl *iopSide3_;
	AMReadOnlyPVControl *iopSide4_;
	AMReadOnlyPVControl *iopSide5_;
	AMReadOnlyPVControl *iopSide6_;
	AMReadOnlyPVControl *iopSide7_;

	// Flow transducer controls

	AMReadOnlyPVwStatusControl *flt5_;
	AMReadOnlyPVwStatusControl *flt6_;
	AMReadOnlyPVwStatusControl *flt7_;
	AMReadOnlyPVwStatusControl *flt8_;
	AMReadOnlyPVwStatusControl *flt9_;
	AMReadOnlyPVwStatusControl *flt10_;
	AMReadOnlyPVwStatusControl *flt11_;
	AMReadOnlyPVwStatusControl *flt12_;
	AMReadOnlyPVwStatusControl *flt13_;
	AMReadOnlyPVwStatusControl *flt14_;
	AMReadOnlyPVwStatusControl *flt15_;
	AMReadOnlyPVwStatusControl *flt16_;
	AMReadOnlyPVwStatusControl *flt17_;
	AMReadOnlyPVwStatusControl *flt18_;
	AMReadOnlyPVwStatusControl *flt19_;
	AMReadOnlyPVwStatusControl *flt20_;

	// Flow switch controls


	// Temperature monitor controls

};

#endif // BIOXASSIDEBEAMLINEUTILITIES_H