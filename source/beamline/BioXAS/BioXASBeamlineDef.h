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

	/// check whether a given motor type is pseudo motor or not
	inline bool isPseudoMotor(int type)
	{
		return (type == PseudoFilterMotor) || (type == PseudoM1Motor)
			|| (type == PseudoMaskMotor)|| (type == PseudoMonoMotor)
			|| (type == PseudoM2Motor);
	}
}

#endif // BIOXASBEAMLINEDEF_H
