#ifndef BIOXASBEAMLINEDEF_H
#define BIOXASBEAMLINEDEF_H

namespace BioXASBeamlineDef {
	/* The defintions of the BioXAS beamline */
	enum BioXASBeamLineID {
		BioXASMainBeamLine = 0,
		BioXASSideBeamLine,
		BioXASImagingBeamLine
	};

	/* The types of the BioXAS beamline motors, which are based on the physical devices, such as beamline filter, Mono, etc */
	enum BioXASMotorType {
		FilterMotor = 0,
		M1Motor,
		MaskMotor,
		MonoMotor,
		M2Motor,
		PseudoFilterMotor,
		PseudoM1Motor,
		PseudoMaskMotor,
		PseudoMonoMotor,
		PseudoM2Motor
	};
}

#endif // BIOXASBEAMLINEDEF_H
