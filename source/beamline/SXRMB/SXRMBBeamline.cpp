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


#include "SXRMBBeamline.h"

#include <QDebug>

SXRMBBeamline::SXRMBBeamline()
	: AMBeamline("SXRMB Beamline")
{
	energy_ = new AMPVwStatusControl("Energy", "BL1606-B1-1:Energy:fbk", "BL1606-B1-1:Energy", "BL1606-B1-1:Energy:status", QString(), this, 0.1, 2.0, new AMControlStatusCheckerCLSMAXv());

	beamlineStatus_ = new AMReadOnlyPVControl("BeamlineStatus", "BL1606-B01:ready:status", this);

	microprobeSampleStageX_ = new AMPVwStatusControl("MicroprobeSampleStageX", "SVM1606-5-B10-07:mm:sp", "SVM1606-5-B10-07:mm", "SVM1606-5-B10-07:status", "SVM1606-5-B10-07:stop", this, 0.1, 2.0, new AMControlStatusCheckerCLSMAXv());
	microprobeSampleStageY_ = new AMPVwStatusControl("MicroprobeSampleStageY", "SVM1606-5-B10-08:mm:sp", "SVM1606-5-B10-08:mm", "SVM1606-5-B10-08:status", "SVM1606-5-B10-08:stop", this, 0.1, 2.0, new AMControlStatusCheckerCLSMAXv());
	microprobeSampleStageZ_ = new AMPVwStatusControl("MicroprobeSampleStageZ", "SVM1606-5-B10-09:mm:sp", "SVM1606-5-B10-09:mm", "SVM1606-5-B10-09:status", "SVM1606-5-B10-09:stop", this, 0.1, 2.0, new AMControlStatusCheckerCLSMAXv());

	microprobeSampleStageControlSet_ = new AMControlSet(this);
	microprobeSampleStageControlSet_->addControl(microprobeSampleStageX_);
	microprobeSampleStageControlSet_->addControl(microprobeSampleStageY_);
	microprobeSampleStageControlSet_->addControl(microprobeSampleStageZ_);

	setupSynchronizedDwellTime();
	setupComponents();
	setupDiagnostics();
	setupSampleStage();
	setupDetectors();
	setupControlSets();
	setupMono();
	setupMotorGroup();
	setupControlsAsDetectors();
	setupExposedControls();
	setupExposedDetectors();

	connect(energy_, SIGNAL(connected(bool)), this, SLOT(onEnergyPVConnected(bool)));
	connect(beamlineStatus_, SIGNAL(connected(bool)), this, SLOT(onBeamlineStatusPVConnected(bool)));
	connect(microprobeSampleStageControlSet_, SIGNAL(connected(bool)), this, SLOT(onMicroprobeSampleStagePVsConnected(bool)));

	wasConnected_ = false;
	connectedHelper();
}

CLSSIS3820Scaler* SXRMBBeamline::scaler() const {
	return scaler_;
}

AMPVwStatusControl* SXRMBBeamline::energy() const {
	return energy_;
}

AMPVwStatusControl* SXRMBBeamline::microprobeSampleStageX() const {
	return microprobeSampleStageX_;
}

AMPVwStatusControl* SXRMBBeamline::microprobeSampleStageY() const {
	return microprobeSampleStageY_;
}

AMPVwStatusControl* SXRMBBeamline::microprobeSampleStageZ() const {
	return microprobeSampleStageZ_;
}

AMReadOnlyPVControl* SXRMBBeamline::beamlineStatus() const {
	return beamlineStatus_;
}

bool SXRMBBeamline::isConnected() const{
	return energy_->isConnected() && beamlineStatus_->isConnected() && microprobeSampleStageControlSet_->isConnected();
}

void SXRMBBeamline::setupDiagnostics()
{

}

void SXRMBBeamline::setupSampleStage()
{

}

void SXRMBBeamline::setupMotorGroup()
{
}

void SXRMBBeamline::setupDetectors()
{

}

void SXRMBBeamline::setupControlSets()
{

}

void SXRMBBeamline::setupMono()
{

}

void SXRMBBeamline::setupSynchronizedDwellTime()
{

}

void SXRMBBeamline::setupComponents()
{

}

void SXRMBBeamline::setupControlsAsDetectors()
{

}

void SXRMBBeamline::setupExposedControls()
{

}

void SXRMBBeamline::setupExposedDetectors()
{

}

SXRMBBeamline::~SXRMBBeamline()
{

}

void SXRMBBeamline::connectedHelper(){
	if (wasConnected_ && !isConnected()) {
		emit connected(false);
		wasConnected_ = false;
	} else if (!wasConnected_ && isConnected()) {
		emit connected(true);
		wasConnected_ = true;
	}
}

void SXRMBBeamline::onEnergyPVConnected(bool) {
	connectedHelper();
}

void SXRMBBeamline::onBeamlineStatusPVConnected(bool) {
	connectedHelper();
}

void SXRMBBeamline::onMicroprobeSampleStagePVsConnected(bool) {
	connectedHelper();
}
