#include "VESPERSExperimentConfiguration.h"

VESPERSExperimentConfiguration::VESPERSExperimentConfiguration(CLSSynchronizedDwellTime *dwellTime, SampleStageControl *pseudoSampleStage, XRFDetector *vortex1E, XRFDetector *vortex4E, QObject *parent)
	: QObject(parent)
{
	synchronizedDwellTime_ = dwellTime;
	pseudoSampleStage_ = pseudoSampleStage;
	vortex1E_ = vortex1E;
	vortex4E_ = vortex4E;

	poeBeamStatus_ = new AMReadOnlyPVControl("POE Beam Status", "07B2:POE_BeamStatus", this);
	poeBeamStatusEnable_ = new AMSinglePVControl("POE Beam Status Enable", "07B2:EnablePOEStat", this, 0.1);
	soeBeamStatus_ = new AMReadOnlyPVControl("SOE Beam Status", "07B2:SOE_BeamStatus", this);
	soeBeamStatusEnable_ = new AMSinglePVControl("SOE Beam Status Enable", "07B2:EnableSOEStat", this, 0.1);
	fastShutterReady_ = new AMReadOnlyPVControl("Fast Shutter Ready", "DIO1607-01:CCD:NotReady:fbk", this);
	ccdStatus_ = new AMReadOnlyPVControl("CCD Status", "IOC1607-003:det1:DetectorState_RBV", this);

	type_ = Custom;

	usePOE_ = false;
	useSOE_ = false;
	useFastShutter_ = false;
	useCCD_ = false;
	useSampleStage_ = false;
	useSingleEl_ = false;
	useFourEl_ = false;

	connect(poeBeamStatusEnable_, SIGNAL(valueChanged(double)), this, SLOT(onPOEEnableChanged(double)));
	connect(soeBeamStatusEnable_, SIGNAL(valueChanged(double)), this, SLOT(onSOEEnableChanged(double)));
	connect(synchronizedDwellTime_, SIGNAL(connected(bool)), this, SLOT(onSynchronizedDwellTimeStartup(bool)));

	connect(poeBeamStatus_, SIGNAL(valueChanged(double)), this, SLOT(determineExperimentStatus()));
	connect(soeBeamStatus_, SIGNAL(valueChanged(double)), this, SLOT(determineExperimentStatus()));
	connect(fastShutterReady_, SIGNAL(valueChanged(double)), this, SLOT(determineExperimentStatus()));
	connect(ccdStatus_, SIGNAL(connected(bool)), this, SLOT(determineExperimentStatus()));
	connect(pseudoSampleStage_, SIGNAL(connected(bool)), this, SLOT(determineExperimentStatus()));
	connect(vortex1E_, SIGNAL(connected(bool)), this, SLOT(determineExperimentStatus()));
	connect(vortex4E_, SIGNAL(connected(bool)), this, SLOT(determineExperimentStatus()));
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
	if (useSOE_ && (int)soeBeamStatus_->value() == 0)
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
		useSOEStatus(true);
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
		useSOEStatus(true);
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
		useSOEStatus(true);
		useFastShutterStatus(false);
		useCCDStatus(false);
		useSampleStageStatus(true);
		useSingleElementVortex(false);
		useFourElementVortex(true);
		break;
	case XRFw1elAndXRD:
		// XRF w/ 1-el Vortex + CCD.  Enable: POE, SOE, 1-el Vortex, CCD, Fast Shutter, and Sample Stage
		usePOEStatus(true);
		useSOEStatus(true);
		useFastShutterStatus(true);
		useCCDStatus(true);
		useSampleStageStatus(true);
		useSingleElementVortex(true);
		useFourElementVortex(false);
		break;
	case XRFw4elAndXRD:
		// XRF w/ 4-el Vortex.  Enable: POE, SOE, 4-el Vortex, CCD, Fast Shutter, and Sample Stage
		usePOEStatus(true);
		useSOEStatus(true);
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

	emit POEStatusChanged(use);
	determineExperimentStatus();
}

void VESPERSExperimentConfiguration::useSOEStatus(bool use)
{
	if (useSOE_ == use)
		return;

	useSOE_ = use;

	if (soeBeamStatusEnable_->isConnected())
		soeBeamStatusEnable_->move(use == true ? 0.0 : 1.0);

	emit SOEStatusChanged(use);
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
