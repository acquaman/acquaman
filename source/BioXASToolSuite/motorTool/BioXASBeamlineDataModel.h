#ifndef BIOXASBEAMLINEDATAMODEL_H
#define BIOXASBEAMLINEDATAMODEL_H

#include <QObject>

#include "BioXASBeamline.h"
#include "BioXASMAXvMotor.h"

class BioXASBeamlineDataModel : public QObject
{
	Q_OBJECT

public:
	static BioXASBeamlineDataModel* instance() {
		if (!instance_) {
			instance_ = new BioXASBeamlineDataModel();
		}

		return instance_;
	}

	~BioXASBeamlineDataModel();

private:
	static BioXASBeamlineDataModel* instance_;

public:
	QList<BioXASMAXvMotor *> getBeamlineMotorsByMotorType(BioXASBeamlineDef::BioXASBeamLineID beamline, BioXASBeamlineDef::BioXASMotorType motorType);

private:
	/* This data model is a singleton pattern */
	explicit BioXASBeamlineDataModel(QObject *parent = 0);

	BioXASBeamline *bioXASMainBeamline_;
	BioXASBeamline *bioXASSideBeamline_;
	BioXASBeamline *bioXASImagingBeamline_;

	/* The list of motor definitions of each BioXAS beamline */
	QList<BioXASBeamlineDef::BioXASMotor> bioXASMainBeamlineMotorDefTable();
	QList<BioXASBeamlineDef::BioXASMotor> bioXASSideBeamlineMotorDefTable();
	QList<BioXASBeamlineDef::BioXASMotor> bioXASImagingBeamlineMotorDefTable();
};

#endif // BIOXASBEAMLINEDATAMODEL_H
