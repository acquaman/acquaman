#ifndef BEAMLINEMOTORS_H
#define BEAMLINEMOTORS_H

#include <QObject>

#include "BioXASMAXvMotor.h"

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
		M2Motor
	};

	/* The defintions of a BioXAS motor*/
	struct BioXASMotor {
		BioXASMotorType motorType_;   // The type of the motor, it should be one of the values of enum BioXASMotorType
		QString motorTypeName_;       // The display name of the motor type
		QString pvBaseName_;          // The base name of the motor PV. It should be everthing before the first ":"
		QString pvUnitField_;         // The unit field of the PV, like ":mm", ":lbs", ":deg", ":V", etc
		QString description_;         // The description of this PV

		BioXASMotor(BioXASMotorType type, QString nameOfType, QString baseName, QString pvUnitField, QString desc) {
			motorType_ = type;
			motorTypeName_ = nameOfType;
			pvBaseName_ = baseName;
			pvUnitField_ = pvUnitField;
			description_ = desc;
		}
	};
}

/*
 The defintions of a beamline, For this motor tool, it contains only motor information.
 In the future, it can be extended to contain more information.
 For that case, it will be better to move this file from the "motorTool" folder to the upper level folder
*/
class BioXASBeamline: public QObject
{
	Q_OBJECT
public:
	explicit BioXASBeamline(QList<BioXASBeamlineDef::BioXASMotor> motorDefinitions, QObject *parent = 0);
	~BioXASBeamline();

	/* get a list of the motors by a given type, such as FilterMotor, M1Motor, etc */
	QList<BioXASMAXvMotor *> getMotorsByType(BioXASBeamlineDef::BioXASMotorType motorType);

private:
	QList<BioXASMAXvMotor *> motors_; // the list of CLSMAXvMotor for BioXAS beamline
};

#endif // BEAMLINEMOTORS_H
