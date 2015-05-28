#include "BioXASFourElementVortexDetector.h"

BioXASFourElementVortexDetector::BioXASFourElementVortexDetector(const QString &name, const QString &description, QObject *parent)
	: AMXspress3XRFDetector(name, description, parent)
{
	// Stuff required by AMXRFDetector.
	acquireControl_ = new AMSinglePVControl("Acquisition Time", "BL1607-5-I22:SetPort_1", this, 0.5);
	acquisitionStatusControl_ = new AMReadOnlyPVControl("Status", "DXP1607-801:DetectorState_RBV", this);
	acquireTimeControl_ = new AMSinglePVControl("Integration Time", "BL1607-5-I22:SetPort_1.HIGH", this, 0.001);
	elapsedTimeControl_ = 0;

	for (int i = 0; i < 4; i++){

		channelEnableControls_.append(new AMSinglePVControl(QString("Channel Enable %1").arg(i+1), QString("DXP1607-801:C%1_PluginControlVal").arg(i+1), this, 0.1));
		spectraControls_.append(new AMReadOnlyPVControl(QString("Raw Spectrum %1").arg(i+1), QString("DXP1607-801:ARR%1:ArrayData").arg(i+1), this));
		thresholdControls_.append(new AMPVControl(QString("Threshold %1").arg(i+1), QString("DXP1607-801:C%1_SCA4_THRESHOLD_RBV").arg(i+1), QString("DXP1607-801:C%1_SCA4_THRESHOLD").arg(i+1), QString(), this, 0.5));
	}

	initializationControl_ = new AMSinglePVControl("Initialization", "DXP1607-801:Acquire", this, 0.1);
	eraseControl_ = new AMSinglePVControl("EraseControl", "DXP1607-801:ERASE", this, 0.5);
	updateControl_ = new AMSinglePVControl("UpdateControl", "DXP1607-801:UPDATE", this, 0.5);
	statusMessageControl_ = new AMReadOnlyPVControl("StatusMessage", "DXP1607-801:StatusMessage_RBV", this);
	currentFrameCountControl_ = new AMReadOnlyPVControl("CurrentFrame", "DXP1607-801:ArrayCounter_RBV", this);
	framesPerAcquisitionControl_ = new AMPVControl("FramesPerAcquisition", "DXP1607-801:NumImages_RBV", "DXP1607-801:NumImages", QString(), this, 0.5);

	makeConnections();
}

BioXASFourElementVortexDetector::~BioXASFourElementVortexDetector()
{

}
