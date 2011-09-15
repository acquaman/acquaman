#include "VESPERSExperimentConfiguration.h"

VESPERSExperimentConfiguration::VESPERSExperimentConfiguration(CLSSynchronizedDwellTime *dwellTime, QObject *parent)
	: QObject(parent)
{
	poeBeamStatus_ = new AMReadOnlyPVControl("POE Beam Status", "07B2:POE_BeamStatus", this);
	poeBeamStatusEnable_ = new AMSinglePVControl("POE Beam Status Enable", "07B2:EnablePOEStat", this, 0.1);
	soeBeamStatus_ = new AMReadOnlyPVControl("SOE Beam Status", "07B2:SOE_BeamStatus", this);
	soeBeamStatusEnable_ = new AMSinglePVControl("SOE Beam Status Enable", "07B2:EnableSOEStat", this, 0.1);
	fastShutterReady_ = new AMReadOnlyPVControl("Fast Shutter Ready", "DIO1607-01:CCD:NotReady:fbk", this);
	ccdStatus_ = new AMReadOnlyPVControl("CCD Status", "IOC1607-003:det1:DetectorState_RBV", this);

	usePOE_ = false;
	useSOE_ = false;
	useFastShutter_ = false;
	useCCD_ = false;
	useSampleStage_ = false;
	useSingleEl_ = false;
	useFourEl_ = false;

	connect(poeBeamStatus_, SIGNAL(valueChanged(double)), this, SLOT(determineExperimentStatus()));
	connect(soeBeamStatus_, SIGNAL(valueChanged(double)), this, SLOT(determineExperimentStatus()));
	connect(fastShutterReady_, SIGNAL(valueChanged(double)), this, SLOT(determineExperimentStatus()));
	connect(ccdStatus_, SIGNAL(valueChanged(double)), this, SLOT(determineExperimentStatus()));
	connect(pseudoSampleStage_, SIGNAL(connected(bool)), this, SLOT(determineExperimentStatus()));
	connect(((XRFDetector *)vortex1E_), SIGNAL(connected(bool)), this, SLOT(determineExperimentStatus()));
	connect(((XRFDetector *)vortex4E_), SIGNAL(connected(bool)), this, SLOT(determineExperimentStatus()));
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

