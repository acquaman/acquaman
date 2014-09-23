#ifndef BEAMLINEMOTORS_H
#define BEAMLINEMOTORS_H

#include <QObject>

#include "BioXASMAXvMotor.h"

namespace BioXASBeamlineDef {
	enum BioXASBeamLineID {
		BioXASMainBeamLine = 0,
		BioXASSideBeamLine,
		BioXASImagingBeamLine
	};

	enum BioXASMotorType {
		FilterMotor = 0,
		M1Motor,
		MaskMotor,
		MonoMotor,
		M2Motor
	};

	struct BioXASMotor {
		BioXASMotorType motorCategory_;
		QString motorCategoryName_;
		QString pvBaseName_;
		QString pvSubName_;
		QString description_;

		BioXASMotor(BioXASMotorType category, QString categoryName, QString baseName, QString pvSubName, QString desc) {
			motorCategory_ = category;
			motorCategoryName_ = categoryName;
			pvBaseName_ = baseName;
			pvSubName_ = pvSubName;
			description_ = desc;
		}
	};
}

class BioXASBeamline: public QObject
{
	Q_OBJECT
public:
	explicit BioXASBeamline(QList<BioXASBeamlineDef::BioXASMotor> motorDefinitions, QObject *parent = 0);

	QList<BioXASMAXvMotor *> getMotorsByCategory(BioXASBeamlineDef::BioXASMotorType category);

private:
	QList<BioXASMAXvMotor *> motors_;

};

#endif // BEAMLINEMOTORS_H
