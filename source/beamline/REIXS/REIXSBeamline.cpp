/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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

#include "REIXSBeamline.h"



REIXSBeamline::REIXSBeamline() :
	AMBeamline("REIXSBeamline")
{
	// Spectromter: controls and control set for positioners:
	spectrometer_ = new REIXSSpectrometer(this);
	addChildControl(spectrometer_);
	spectrometerPositionSet_ = new AMControlSet(this);
	spectrometerPositionSet_->addControl(spectrometer()->angleDrive());
	spectrometerPositionSet_->addControl(spectrometer()->detectorTranslation());
	spectrometerPositionSet_->addControl(spectrometer()->detectorTiltDrive());
	spectrometerPositionSet_->addControl(spectrometer()->detectorRotationDrive());
	spectrometerPositionSet_->addControl(spectrometer()->hexapod()->x());
	spectrometerPositionSet_->addControl(spectrometer()->hexapod()->y());
	spectrometerPositionSet_->addControl(spectrometer()->hexapod()->z());
	spectrometerPositionSet_->addControl(spectrometer()->hexapod()->u());
	spectrometerPositionSet_->addControl(spectrometer()->hexapod()->v());
	spectrometerPositionSet_->addControl(spectrometer()->hexapod()->w());
	spectrometerPositionSet_->addControl(spectrometer()->hexapod()->r());
	spectrometerPositionSet_->addControl(spectrometer()->hexapod()->s());
	spectrometerPositionSet_->addControl(spectrometer()->hexapod()->t());

	// Sample Chamber: controls and control set for positioners:
	sampleChamber_ = new REIXSSampleChamber(this);
	addChildControl(sampleChamber_);
	sampleManipulatorSet_ = new AMControlSet(this);

	sampleManipulatorSet_->addControl(sampleChamber()->x());
	sampleManipulatorSet_->addControl(sampleChamber()->y());
	sampleManipulatorSet_->addControl(sampleChamber()->z());
	sampleManipulatorSet_->addControl(sampleChamber()->r());

	// MCP detector
	mcpDetector_ = new REIXSXESMCPDetector("xesImage", "MCD1610-401", this);

}




REIXSHexapod::REIXSHexapod(QObject* parent)
	: AMControl("hexapod", "n/a", parent) {

	QString baseName = "HXPD1610-401-01:";
	x_ = new AMPVwStatusControl("hexapodX", baseName+"X:sp", baseName+"X", baseName+"moving", QString(), this, 0.1);
	y_ = new AMPVwStatusControl("hexapodY", baseName+"Y:sp", baseName+"Y", baseName+"moving", QString(), this, 0.1);
	z_ = new AMPVwStatusControl("hexapodZ", baseName+"Z:sp", baseName+"Z", baseName+"moving", QString(), this, 0.1);
	u_ = new AMPVwStatusControl("hexapodU", baseName+"U:sp", baseName+"U", baseName+"moving", QString(), this, 0.5);
	v_ = new AMPVwStatusControl("hexapodV", baseName+"V:sp", baseName+"V", baseName+"moving", QString(), this, 0.5);
	w_ = new AMPVwStatusControl("hexapodW", baseName+"W:sp", baseName+"W", baseName+"moving", QString(), this, 0.5);
	r_ = new AMPVwStatusControl("hexapodR", baseName+"R:sp", baseName+"R", baseName+"moving", QString(), this, 0.1);
	s_ = new AMPVwStatusControl("hexapodS", baseName+"S:sp", baseName+"S", baseName+"moving", QString(), this, 0.1);
	t_ = new AMPVwStatusControl("hexapodT", baseName+"T:sp", baseName+"T", baseName+"moving", QString(), this, 0.1);

	addChildControl(x_);
	addChildControl(y_);
	addChildControl(z_);
	addChildControl(u_);
	addChildControl(v_);
	addChildControl(w_);
	addChildControl(r_);
	addChildControl(s_);
	addChildControl(t_);
}

REIXSSpectrometer::REIXSSpectrometer(QObject *parent)
	: AMControl("spectrometer", "n/a", parent) {

	angleDrive_ = new AMPVwStatusControl("angleDrive",
										 "SMTR1610-4-I21-01:mm:sp",
										 "SMTR1610-4-I21-01:mm",
										 "SMTR1610-4-I21-01:status",
										 "SMTR1610-4-I21-01:stop", this, 1);

	detectorTranslation_ = new AMPVwStatusControl("detectorTranslation",
												  "SMTR1610-4-I21-04:mm:sp",
												  "SMTR1610-4-I21-04:mm",
												  "SMTR1610-4-I21-04:status",
												  "SMTR1610-4-I21-04:stop", this, 1);

	detectorTiltDrive_ = new AMPVwStatusControl("detectorTiltDrive",
												  "SMTR1610-4-I21-02:mm:sp",
												  "SMTR1610-4-I21-02:mm",
												  "SMTR1610-4-I21-02:status",
												  "SMTR1610-4-I21-02:stop", this, 0.5);

	detectorRotationDrive_ = new AMPVwStatusControl("detectorRotationDrive",
												  "SMTR1610-4-I21-03:mm:sp",
												  "SMTR1610-4-I21-03:mm",
												  "SMTR1610-4-I21-03:status",
												  "SMTR1610-4-I21-03:stop", this, 0.5);

	hexapod_ = new REIXSHexapod(this);

	addChildControl(angleDrive_);
	addChildControl(detectorTranslation_);
	addChildControl(detectorTiltDrive_);
	addChildControl(detectorRotationDrive_);
	addChildControl(hexapod_);

}


REIXSSampleChamber::REIXSSampleChamber(QObject *parent)
	: AMControl("sampleChamber", "n/a", parent) {

	x_ = new AMPVwStatusControl("sampleX",
								"SMTR1610-401-X:step:sp",
								"SMTR1610-401-X:step",
								"SMTR1610-401-X:moving",
								"SMTR1610-401-X:stop", this, 1);
	y_ = new AMPVwStatusControl("sampleY",
								"SMTR1610-401-Y:step:sp",
								"SMTR1610-401-Y:step",
								"SMTR1610-401-Y:moving",
								"SMTR1610-401-Y:stop", this, 1);
	z_ = new AMPVwStatusControl("sampleZ",
								"SMTR1610-401-Z:step:sp",
								"SMTR1610-401-Z:step",
								"SMTR1610-401-Z:moving",
								"SMTR1610-401-Z:stop", this, 1);
	r_ = new AMPVwStatusControl("sampleTheta",
								"SMTR1610-401-R:step:sp",
								"SMTR1610-401-R:step",
								"SMTR1610-401-R:moving",
								"SMTR1610-401-R:stop", this, 1);
	loadLockZ_ = new AMPVwStatusControl("loadLockZ",
										"SMTR1610-401-LZ:step:sp",
										"SMTR1610-401-LZ:step",
										"SMTR1610-401-LZ:moving",
										"SMTR1610-401-LZ:stop", this, 1);
	loadLockR_ = new AMPVwStatusControl("loadLockTheta",
											"SMTR1610-401-LR:step:sp",
											"SMTR1610-401-LR:step",
											"SMTR1610-401-LR:moving",
											"SMTR1610-401-LR:stop", this, 1);

}

