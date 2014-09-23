/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef MOTORMODEL_H
#define MOTORMODEL_H

#include <QObject>

#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "source/actions3/AMAction3.h"
#include "source/actions3/actions/AMControlMoveAction3.h"
#include "source/actions3/AMListAction3.h"

#include "BioXASBeamline.h"
#include "BioXASMAXvMotor.h"

class MotorModel : public QObject
{
	Q_OBJECT

public:
	static MotorModel* instance() {
		if (!instance_) {
			instance_ = new MotorModel();
		}

		return instance_;
	}

	~MotorModel();

private:
	static MotorModel* instance_;

signals:

public:
	QList<BioXASMAXvMotor *> getMotorsByCategory(BioXASBeamlineDef::BioXASBeamLineID beamline, BioXASBeamlineDef::BioXASMotorType category);

public slots:

protected slots:

private:
	explicit MotorModel(QObject *parent = 0);

	BioXASBeamline *bioXASMainBeamlineMotors_;
	BioXASBeamline *bioXASSideBeamlineMotors_;
	BioXASBeamline *bioXASImagingBeamlineMotors_;

	void createComponents();

	QList<BioXASBeamlineDef::BioXASMotor> bioXASMainBeamlineMotorDefTable();
	QList<BioXASBeamlineDef::BioXASMotor> bioXASSideBeamlineMotorDefTable();
	QList<BioXASBeamlineDef::BioXASMotor> bioXASImagingBeamlineMotorDefTable();
};

#endif // MOTORMODEL_H
