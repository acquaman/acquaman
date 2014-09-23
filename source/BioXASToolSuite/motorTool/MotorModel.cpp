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


#include "MotorModel.h"
#include <QDebug>

MotorModel* MotorModel::instance_ = 0;

MotorModel::MotorModel(QObject *parent) : QObject(parent)
{
	createComponents();
}

MotorModel::~MotorModel() {
	delete bioXASMainBeamlineMotors_;
	delete bioXASSideBeamlineMotors_;
	delete bioXASImagingBeamlineMotors_;
}

QList<BioXASMAXvMotor *> MotorModel::getMotorsByCategory(BioXASBeamlineDef::BioXASBeamLineID beamline, BioXASBeamlineDef::BioXASMotorType category)
{
	QList<BioXASMAXvMotor *> matchMotors;

	if (beamline < BioXASBeamlineDef::BioXASMainBeamLine || beamline > BioXASBeamlineDef::BioXASImagingBeamLine) {
		qDebug() << "ERROR: invalid BioXAS beamline: " << beamline;
		return matchMotors;
	}

	if (category < BioXASBeamlineDef::FilterMotor || category > BioXASBeamlineDef::M2Motor) {
		qDebug() << "ERROR: invalid BioXAS Motor category: " << category;
		return matchMotors;
	}

	BioXASBeamline * beamlineMotors;

	switch (beamline) {
	case BioXASBeamlineDef::BioXASMainBeamLine:
		beamlineMotors = bioXASMainBeamlineMotors_;
		break;
	case BioXASBeamlineDef::BioXASSideBeamLine:
		beamlineMotors = bioXASSideBeamlineMotors_;
		break;
	case BioXASBeamlineDef::BioXASImagingBeamLine:
		beamlineMotors = bioXASImagingBeamlineMotors_;
		break;
	default:
		beamlineMotors = 0;
		break;
	}

	if (beamlineMotors) {
		matchMotors = beamlineMotors->getMotorsByCategory(category);
	}

	return matchMotors;
}


void MotorModel::createComponents()
{
	bioXASMainBeamlineMotors_ = new BioXASBeamline(bioXASMainBeamlineMotorDefTable());
	bioXASSideBeamlineMotors_ = new BioXASBeamline(bioXASSideBeamlineMotorDefTable());
	bioXASImagingBeamlineMotors_ = new BioXASBeamline(bioXASImagingBeamlineMotorDefTable());
}

QList<BioXASBeamlineDef::BioXASMotor> MotorModel::bioXASMainBeamlineMotorDefTable() {
	QList<BioXASBeamlineDef::BioXASMotor> beamlineMotors;
	beamlineMotors  << BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::FilterMotor, QString("Carbon Filter Farm"), QString("SMTR1607-5-I00-03"), QString("SMTR1607-5-I00-03 Filter 1"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::FilterMotor, QString("Carbon Filter Farm"), QString("SMTR1607-5-I00-04"), QString("SMTR1607-5-I00-04 Filter 2"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M1Motor,     QString("Main M1"),            QString("SMTR1607-5-I21-01"), QString("SMTR1607-5-I21-01 VERT INB (UPSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M1Motor,     QString("Main M1"),            QString("SMTR1607-5-I21-02"), QString("SMTR1607-5-I21-02 VERT OUTB (UPSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M1Motor,     QString("Main M1"),            QString("SMTR1607-5-I21-03"), QString("SMTR1607-5-I21-03 VERT (DOWNSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M1Motor,     QString("Main M1"),            QString("SMTR1607-5-I21-04"), QString("SMTR1607-5-I21-04 STRIPE SELECT"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M1Motor,     QString("Main M1"),            QString("SMTR1607-5-I21-05"), QString("SMTR1607-5-I21-05 YAW"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M1Motor,     QString("Main M1"),            QString("SMTR1607-5-I21-06"), QString("SMTR1607-5-I21-06 BENDER (UPSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M1Motor,     QString("Main M1"),            QString("SMTR1607-5-I21-07"), QString("SMTR1607-5-I21-07 BENDER (DOWNSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M1Motor,     QString("Main M1"),            QString("SMTR1607-5-I21-08"), QString("SMTR1607-5-I21-08 UPPER SLIT BLADE"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MaskMotor,   QString("Main Variable Mask"), QString("SMTR1607-5-I21-09"), QString("SMTR1607-5-I21-09 VERT UPPER BLADE"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MaskMotor,   QString("Main Variable Mask"), QString("SMTR1607-5-I21-10"), QString("SMTR1607-5-I21-10 VERT LOWER BLADE"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Main Mono"),          QString("SMTR1607-5-I21-11"), QString("SMTR1607-5-I21-11 PHOSPHOR PADDLE"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Main Mono"),          QString("SMTR1607-5-I21-12"), QString("SMTR1607-5-I21-12 BRAGG"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Main Mono"),          QString("SMTR1607-5-I21-13"), QString("SMTR1607-5-I21-13 VERTICAL"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Main Mono"),          QString("SMTR1607-5-I21-14"), QString("SMTR1607-5-I21-14 LATERAL"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Main Mono"),          QString("SMTR1607-5-I21-22"), QString("SMTR1607-5-I21-22 XTAL XCHAGE"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Main Mono"),          QString("SMTR1607-5-I21-23"), QString("SMTR1607-5-I21-23 XTAL 1 PITCH"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Main Mono"),          QString("SMTR1607-5-I21-24"), QString("SMTR1607-5-I21-24 XTAL 1 ROLL"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Main Mono"),          QString("SMTR1607-5-I21-25"), QString("SMTR1607-5-I21-25 XTAL 2 PITCH"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Main Mono"),          QString("SMTR1607-5-I21-26"), QString("SMTR1607-5-I21-26 XTAL 2 ROLL"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M2Motor,     QString("Main M2"),            QString("SMTR1607-5-I21-15"), QString("SMTR1607-5-I21-15 VERT INB (UPSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M2Motor,     QString("Main M2"),            QString("SMTR1607-5-I21-16"), QString("SMTR1607-5-I21-16 VERT OUTB (UPSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M2Motor,     QString("Main M2"),            QString("SMTR1607-5-I21-17"), QString("SMTR1607-5-I21-17 VERT (DOWNSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M2Motor,     QString("Main M2"),            QString("SMTR1607-5-I21-18"), QString("SMTR1607-5-I21-18 STRIPE SELECT"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M2Motor,     QString("Main M2"),            QString("SMTR1607-5-I21-19"), QString("SMTR1607-5-I21-19 YAW"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M2Motor,     QString("Main M2"),            QString("SMTR1607-5-I21-20"), QString("SMTR1607-5-I21-20 BENDER (UPSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M2Motor,     QString("Main M2"),            QString("SMTR1607-5-I21-21"), QString("SMTR1607-5-I21-21 BENDER (DOWNSTREAM)"));

	return beamlineMotors;
}

QList<BioXASBeamlineDef::BioXASMotor> MotorModel::bioXASSideBeamlineMotorDefTable() {
	QList<BioXASBeamlineDef::BioXASMotor> beamlineMotors;
	beamlineMotors  << BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::FilterMotor, QString("Carbon Filter Farm"), QString("SMTR1607-5-I00-01"), QString("SMTR1607-5-I00-01 Filter 1"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::FilterMotor, QString("Carbon Filter Farm"), QString("SMTR1607-5-I00-02"), QString("SMTR1607-5-I00-02 Filter 2"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M1Motor,     QString("Side M1"),            QString("SMTR1607-5-I22-01"), QString("SMTR1607-5-I22-01 VERT INB (UPSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M1Motor,     QString("Side M1"),            QString("SMTR1607-5-I22-02"), QString("SMTR1607-5-I22-02 VERT OUTB (UPSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M1Motor,     QString("Side M1"),            QString("SMTR1607-5-I22-03"), QString("SMTR1607-5-I22-03 VERT (DOWNSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M1Motor,     QString("Side M1"),            QString("SMTR1607-5-I22-04"), QString("SMTR1607-5-I22-04 STRIPE SELECT"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M1Motor,     QString("Side M1"),            QString("SMTR1607-5-I22-05"), QString("SMTR1607-5-I22-05 YAW"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M1Motor,     QString("Side M1"),            QString("SMTR1607-5-I22-06"), QString("SMTR1607-5-I22-06 BENDER (UPSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M1Motor,     QString("Side M1"),            QString("SMTR1607-5-I22-07"), QString("SMTR1607-5-I22-07 BENDER (DOWNSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M1Motor,     QString("Side M1"),            QString("SMTR1607-5-I22-08"), QString("SMTR1607-5-I22-08 UPPER SLIT BLADE"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MaskMotor,   QString("Side Variable Mask"), QString("SMTR1607-5-I22-09"), QString("SMTR1607-5-I22-09 VERT UPPER BLADE"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MaskMotor,   QString("Side Variable Mask"), QString("SMTR1607-5-I22-10"), QString("SMTR1607-5-I22-10 VERT LOWER BLADE"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Side Mono"),          QString("SMTR1607-5-I22-11"), QString("SMTR1607-5-I22-11 PHOSPHOR PADDLE"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Side Mono"),          QString("SMTR1607-5-I22-12"), QString("SMTR1607-5-I22-12 BRAGG"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Side Mono"),          QString("SMTR1607-5-I22-13"), QString("SMTR1607-5-I22-13 VERTICAL"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Side Mono"),          QString("SMTR1607-5-I22-14"), QString("SMTR1607-5-I22-14 LATERAL"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Side Mono"),          QString("SMTR1607-5-I22-22"), QString("SMTR1607-5-I22-22 XTAL XCHAGE"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Side Mono"),          QString("SMTR1607-5-I22-23"), QString("SMTR1607-5-I22-23 XTAL 1 PITCH"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Side Mono"),          QString("SMTR1607-5-I22-24"), QString("SMTR1607-5-I22-24 XTAL 1 ROLL"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Side Mono"),          QString("SMTR1607-5-I22-25"), QString("SMTR1607-5-I22-25 XTAL 2 PITCH"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Side Mono"),          QString("SMTR1607-5-I22-26"), QString("SMTR1607-5-I22-26 XTAL 2 ROLL"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M2Motor,     QString("Side M2"),            QString("SMTR1607-5-I22-15"), QString("SMTR1607-5-I22-15 VERT INB (UPSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M2Motor,     QString("Side M2"),            QString("SMTR1607-5-I22-16"), QString("SMTR1607-5-I22-16 VERT OUTB (UPSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M2Motor,     QString("Side M2"),            QString("SMTR1607-5-I22-17"), QString("SMTR1607-5-I22-17 VERT (DOWNSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M2Motor,     QString("Side M2"),            QString("SMTR1607-5-I22-18"), QString("SMTR1607-5-I22-18 STRIPE SELECT"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M2Motor,     QString("Side M2"),            QString("SMTR1607-5-I22-19"), QString("SMTR1607-5-I22-19 YAW"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M2Motor,     QString("Side M2"),            QString("SMTR1607-5-I22-20"), QString("SMTR1607-5-I22-20 BENDER (UPSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M2Motor,     QString("Side M2"),            QString("SMTR1607-5-I22-21"), QString("SMTR1607-5-I22-21 BENDER (DOWNSTREAM)"));

	return beamlineMotors;
}

QList<BioXASBeamlineDef::BioXASMotor> MotorModel::bioXASImagingBeamlineMotorDefTable() {
	QList<BioXASBeamlineDef::BioXASMotor> beamlineMotors;
	beamlineMotors  << BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::FilterMotor, QString("Carbon Filter Farm"),  QString("SMTR1607-5-I00-05"), QString("SMTR1607-5-I00-05 Filter 1"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::FilterMotor, QString("Carbon Filter Farm"),  QString("SMTR1607-5-I00-06"), QString("SMTR1607-5-I00-06 Filter 2"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M1Motor,     QString("Image M1"),            QString("SMTR1607-5-I10-01"), QString("SMTR1607-5-I10-01 VERT INB (UPSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M1Motor,     QString("Image M1"),            QString("SMTR1607-5-I10-02"), QString("SMTR1607-5-I10-02 VERT OUTB (UPSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M1Motor,     QString("Image M1"),            QString("SMTR1607-5-I10-03"), QString("SMTR1607-5-I10-03 VERT (DOWNSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M1Motor,     QString("Image M1"),            QString("SMTR1607-5-I10-04"), QString("SMTR1607-5-I10-04 STRIPE SELECT"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M1Motor,     QString("Image M1"),            QString("SMTR1607-5-I10-05"), QString("SMTR1607-5-I10-05 YAW"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M1Motor,     QString("Image M1"),            QString("SMTR1607-5-I10-06"), QString("SMTR1607-5-I10-06 BENDER (UPSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M1Motor,     QString("Image M1"),            QString("SMTR1607-5-I10-07"), QString("SMTR1607-5-I10-07 BENDER (DOWNSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MaskMotor,   QString("Image Variable Mask"), QString("SMTR1607-5-I10-08"), QString("SMTR1607-5-I10-08 VERT UPPER BLADE"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MaskMotor,   QString("Image Variable Mask"), QString("SMTR1607-5-I10-09"), QString("SMTR1607-5-I10-09 VERT LOWER BLADE"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Image Mono"),          QString("SMTR1607-5-I10-10"), QString("SMTR1607-5-I10-10 CRYSTAL 2 Z"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Image Mono"),          QString("SMTR1607-5-I10-11"), QString("SMTR1607-5-I10-11 CRYSTAL 2 Y"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Image Mono"),          QString("SMTR1607-5-I10-12"), QString("SMTR1607-5-I10-12 CRYSTAL 2 PITCH"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Image Mono"),          QString("SMTR1607-5-I10-13"), QString("SMTR1607-5-I10-13 CRYSTAL 2 ROLL"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Image Mono"),          QString("SMTR1607-5-I10-14"), QString("SMTR1607-5-I10-14 BRAGG"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Image Mono"),          QString("SMTR1607-5-I10-15"), QString("SMTR1607-5-I10-15 VERTICAL (Y)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::MonoMotor,   QString("Image Mono"),          QString("SMTR1607-5-I10-16"), QString("SMTR1607-5-I10-16 LATERAL (X)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M2Motor,     QString("Image M2"),            QString("SMTR1607-5-I10-17"), QString("SMTR1607-5-I10-17 VERT INB (UPSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M2Motor,     QString("Image M2"),            QString("SMTR1607-5-I10-18"), QString("SMTR1607-5-I10-18 VERT OUTB (UPSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M2Motor,     QString("Image M2"),            QString("SMTR1607-5-I10-19"), QString("SMTR1607-5-I10-19 VERT (DOWNSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M2Motor,     QString("Image M2"),            QString("SMTR1607-5-I10-20"), QString("SMTR1607-5-I10-20 STRIPE SELECT"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M2Motor,     QString("Image M2"),            QString("SMTR1607-5-I10-21"), QString("SMTR1607-5-I10-21 YAW"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M2Motor,     QString("Image M2"),            QString("SMTR1607-5-I10-22"), QString("SMTR1607-5-I10-22 BENDER (UPSTREAM)"))
					<< BioXASBeamlineDef::BioXASMotor(BioXASBeamlineDef::M2Motor,     QString("Image M2"),            QString("SMTR1607-5-I10-23"), QString("SMTR1607-5-I10-23 BENDER (DOWNSTREAM)"));

	return beamlineMotors;
}
