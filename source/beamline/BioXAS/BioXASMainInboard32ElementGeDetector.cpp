#include "BioXASMainInboard32ElementGeDetector.h"

BioXASMainInboard32ElementGeDetector::BioXASMainInboard32ElementGeDetector(const QString &name, const QString &description, AMPVControl *triggerControl, BioXASZebraPulseControl *pulseControl, QObject *parent) :
	BioXAS32ElementGeDetector(name, description, triggerControl, pulseControl, parent)
{
	// Stuff required by AMXRFDetector.

	acquisitionStatusControl_ = new AMReadOnlyPVControl("Status", "DXP1607-I21-01:DetectorState_RBV", this);

	for (int i = 0; i < 32; i++) {
		if (i != 2 && i != 15 && i != 21 && i != 26) { // channels 3, 16, 22, and 27 (start 1) are disabled for this detector.
			channelEnableControls_.append(new AMSinglePVControl(QString("Channel Enable %1").arg(i+1), QString("DXP1607-I21-01:C%1_PluginControlVal").arg(i+1), this, 0.1));
			spectraControls_.append(new AMReadOnlyPVControl(QString("Raw Spectrum %1").arg(i+1), QString("DXP1607-I21-01:ARR%1:ArrayData").arg(i+1), this));
			thresholdControls_.append(new AMPVControl(QString("Threshold %1").arg(i+1), QString("DXP1607-I21-01:C%1_SCA4_THRESHOLD_RBV").arg(i+1), QString("DXP1607-I21-01:C%1_SCA4_THRESHOLD").arg(i+1), QString(), this, 0.5));
		}
	}

	initializationControl_ = new AMSinglePVControl("Initialization", "DXP1607-I21-01:Acquire", this, 0.1);
	eraseControl_ = new AMSinglePVControl("EraseControl", "DXP1607-I21-01:ERASE", this, 0.5);
	updateControl_ = new AMSinglePVControl("UpdateControl", "DXP1607-I21-01:UPDATE", this, 0.5);
	statusMessageControl_ = new AMReadOnlyPVControl("StatusMessage", "DXP1607-I21-01:StatusMessage_RBV", this);
	currentFrameCountControl_ = new AMReadOnlyPVControl("CurrentFrame", "DXP1607-I21-01:ArrayCounter_RBV", this);
	framesPerAcquisitionControl_ = new AMPVControl("FramesPerAcquisition", "DXP1607-I21-01:NumImages_RBV", "DXP1607-I21-01:NumImages", QString(), this, 0.5);

	makeConnections();
}

BioXASMainInboard32ElementGeDetector::~BioXASMainInboard32ElementGeDetector()
{

}
