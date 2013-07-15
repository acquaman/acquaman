/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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

#include "VESPERSExperimentConfiguration.h"

VESPERSExperimentConfiguration::VESPERSExperimentConfiguration(CLSSynchronizedDwellTime *dwellTime, VESPERSSampleStageControl *pseudoSampleStage, XRFDetector *vortex1E, XRFDetector *vortex4E, QObject *parent)
	: QObject(parent)
{
	synchronizedDwellTime_ = dwellTime;
	pseudoSampleStage_ = pseudoSampleStage;
	vortex1E_ = vortex1E;
	vortex4E_ = vortex4E;

	poeBeamStatus_ = new AMReadOnlyPVControl("POE Beam Status", "07B2:POE_BeamStatus", this);
	poeBeamStatusEnable_ = new AMSinglePVControl("POE Beam Status Enable", "07B2:EnablePOEStat", this, 0.1);
	fastShutterReady_ = new AMReadOnlyPVControl("Fast Shutter Ready", "DIO1607-01:CCD:NotReady:fbk", this);
	ccdStatus_ = new AMReadOnlyPVControl("CCD Status", "IOC1607-003:det1:DetectorState_RBV", this);

	// Pseudo-motor reset button.
	resetHVNPseudoMotors_ = new AMProcessVariable("TS1607-2-B21-01:HNV:loadOffsets.PROC", false, this);
	resetXYZPseudoMotors_ = new AMProcessVariable("TS1607-2-B21-01:XYZ:loadOffsets.PROC", false, this);

	type_ = Custom;

	usePOE_ = false;
	useFastShutter_ = false;
	useCCD_ = false;
	useSampleStage_ = false;
	useSingleEl_ = false;
	useFourEl_ = false;
	usePseudoMotors_ = true;

	sampleStageControl_ = new AMSinglePVControl("Sample Stage Choice", "BL1607-B2-1:AddOns:sampleStageChoice", this, 0.1);

	connect(poeBeamStatus_, SIGNAL(valueChanged(double)), this, SLOT(onPOEStatusChanged(double)));

	connect(poeBeamStatusEnable_, SIGNAL(valueChanged(double)), this, SLOT(onPOEEnableChanged(double)));
	connect(synchronizedDwellTime_, SIGNAL(connected(bool)), this, SLOT(onSynchronizedDwellTimeStartup(bool)));

	connect(poeBeamStatus_, SIGNAL(valueChanged(double)), this, SLOT(determineExperimentStatus()));
	connect(fastShutterReady_, SIGNAL(valueChanged(double)), this, SLOT(determineExperimentStatus()));
	connect(ccdStatus_, SIGNAL(connected(bool)), this, SLOT(determineExperimentStatus()));
	connect(pseudoSampleStage_, SIGNAL(connected(bool)), this, SLOT(determineExperimentStatus()));
	connect(vortex1E_, SIGNAL(connected(bool)), this, SLOT(determineExperimentStatus()));
	connect(vortex4E_, SIGNAL(connected(bool)), this, SLOT(determineExperimentStatus()));

	connect(sampleStageControl_, SIGNAL(connected(bool)), this, SLOT(onSampleStageChoiceConnected(bool)));
}

void VESPERSExperimentConfiguration::onPOEStatusChanged(double state)
{
	if (usePOE_ && (int(state) == 0))
		emit beamDumped();
}

void VESPERSExperimentConfiguration::onSynchronizedDwellTimeStartup(bool connected)
{
	if (!connected)
		return;

	disconnect(synchronizedDwellTime_, SIGNAL(connected(bool)), this, SLOT(onSynchronizedDwellTimeStartup(bool)));

	if (synchronizedDwellTime_->elementAt(1)->isEnabled())
		useSingleElementVortex(true);
	if (synchronizedDwellTime_->elementAt(2)->isEnabled()){

		useCCDStatus(true);
		useFastShutterStatus(true);
	}
	if (synchronizedDwellTime_->elementAt(4)->isEnabled())
		useFourElementVortex(true);
}

void VESPERSExperimentConfiguration::determineExperimentStatus()
{
	bool ready = true;

	if (usePOE_ && (int)poeBeamStatus_->value() == 0)
		ready = false;
	if (useFastShutter_ && (int)fastShutterReady_->value() == 0)
		ready = false;
	if (useCCD_ && !ccdStatus_->isConnected())
		ready = false;
	if (useSampleStage_ && !pseudoSampleStage_->isConnected())
		ready = false;
	if (useSingleEl_ && !vortex1E_->isConnected())
		ready = false;
	if (useFourEl_ && !vortex4E_->isConnected())
		ready = false;

	emit experimentReady(ready);
}

void VESPERSExperimentConfiguration::setType(VESPERSExperimentConfiguration::ExperimentType type)
 {
	if (type_ == type)
		return;

	switch(type){

	case Custom:
		// No special actions for custom.
		break;
	case XAS:
		// XAS.  Enable: POE, SOE, and Sample Stage
		usePOEStatus(true);
		useFastShutterStatus(false);
		useCCDStatus(false);
		useSampleStageStatus(true);
		useSingleElementVortex(false);
		useFourElementVortex(false);
		break;
	case XASw1el:
		// XAS w/ 1-el Vortex.  Enable: POE, SOE, 1-el Vortex, and Sample Stage
	case XRFw1el:
		// XRF w/ 1-el Vortex.  Enable: POE, SOE, 1-el Vortex, and Sample Stage
		usePOEStatus(true);
		useFastShutterStatus(false);
		useCCDStatus(false);
		useSampleStageStatus(true);
		useSingleElementVortex(true);
		useFourElementVortex(false);
		break;
	case XASw4el:
		// XAS w/ 4-el Vortex.  Enable: POE, SOE, 4-el Vortex, and Sample Stage
	case XRFw4el:
		// XRF w/ 4-el Vortex.  Enable: POE, SOE, 4-el Vortex, and Sample Stage
		usePOEStatus(true);
		useFastShutterStatus(false);
		useCCDStatus(false);
		useSampleStageStatus(true);
		useSingleElementVortex(false);
		useFourElementVortex(true);
		break;
	case XRFw1elAndXRD:
		// XRF w/ 1-el Vortex + CCD.  Enable: POE, SOE, 1-el Vortex, CCD, Fast Shutter, and Sample Stage
		usePOEStatus(true);
		useFastShutterStatus(true);
		useCCDStatus(true);
		useSampleStageStatus(true);
		useSingleElementVortex(true);
		useFourElementVortex(false);
		break;
	case XRFw4elAndXRD:
		// XRF w/ 4-el Vortex.  Enable: POE, SOE, 4-el Vortex, CCD, Fast Shutter, and Sample Stage
		usePOEStatus(true);
		useFastShutterStatus(true);
		useCCDStatus(true);
		useSampleStageStatus(true);
		useSingleElementVortex(false);
		useFourElementVortex(true);
		break;
	}

	type_ = type;
	emit experimentTypeChanged(type_);
}

void VESPERSExperimentConfiguration::usePOEStatus(bool use)
{
	if (usePOE_ == use)
		return;

	usePOE_ = use;

	if (poeBeamStatusEnable_->isConnected())
		poeBeamStatusEnable_->move(use == true ? 0.0 : 1.0);

	emit poeStatusChanged(use);
	determineExperimentStatus();
}

void VESPERSExperimentConfiguration::useFastShutterStatus(bool use)
{
	if (useFastShutter_ == use)
		return;

	useFastShutter_ = use;
	emit fastShutterStatusChanged(use);
	determineExperimentStatus();
}

void VESPERSExperimentConfiguration::useCCDStatus(bool use)
{
	if (useCCD_ == use)
		return;

	useCCD_ = use;

	if (synchronizedDwellTime_->isConnected())
		synchronizedDwellTime_->elementAt(2)->setEnabled(use);

	emit CCDStatusChanged(use);
	determineExperimentStatus();
}

void VESPERSExperimentConfiguration::useSampleStageStatus(bool use)
{
	if (useSampleStage_ == use)
		return;

	useSampleStage_ = use;
	emit sampleStageStatusChanged(use);
	determineExperimentStatus();
}

void VESPERSExperimentConfiguration::useSingleElementVortex(bool use)
{
	if (useSingleEl_ == use)
		return;

	useSingleEl_ = use;

	if (synchronizedDwellTime_->isConnected())
		synchronizedDwellTime_->elementAt(1)->setEnabled(use);

	emit singleElementVortexStatusChanged(use);
	determineExperimentStatus();
}

void VESPERSExperimentConfiguration::useFourElementVortex(bool use)
{
	if (useFourEl_ == use)
		return;

	useFourEl_ = use;

	if (synchronizedDwellTime_->isConnected())
		synchronizedDwellTime_->elementAt(4)->setEnabled(use);

	emit fourElementVortexStatusChanged(use);
	determineExperimentStatus();
}

void VESPERSExperimentConfiguration::usePseudoMotors(bool use)
{
	if (usePseudoMotors_ == use)
		return;

	usePseudoMotors_ = use;

	if (sampleStageControl_->isConnected())
		sampleStageControl_->move(usePseudoMotors_ ? 1 : 0);

	emit sampleStageChoiceChanged(usePseudoMotors_);
}

void VESPERSExperimentConfiguration::onSampleStageChoiceConnected(bool connected)
{
	if (connected){

		if (int(sampleStageControl_->value()) == 0)
			usePseudoMotors_ = false;
		else
			usePseudoMotors_ = true;

		emit sampleStageChoiceChanged(usePseudoMotors_);
	}
}
