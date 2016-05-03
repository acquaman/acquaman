#include "BioXASMainInboard32ElementGeDetector.h"

BioXASMainInboard32ElementGeDetector::BioXASMainInboard32ElementGeDetector(const QString &name, const QString &description, AMPVControl *triggerControl, BioXASZebraPulseControl *pulseControl, QObject *parent) :
	BioXAS32ElementGeDetector(name, description, triggerControl, pulseControl, parent)
{
	// Stuff required by AMXRFDetector.

	acquisitionStatusControl_ = new AMReadOnlyPVControl("Status", "PDTR1607-7-I21-01:DetectorState_RBV", this);

	for (int i = 0; i < 32; i++) {
		channelEnableControls_.append(new AMSinglePVControl(QString("Channel Enable %1").arg(i+1), QString("PDTR1607-7-I21-01:C%1_PluginControlVal").arg(i+1), this, 0.1));
		spectraControls_.append(new AMReadOnlyPVControl(QString("Raw Spectrum %1").arg(i+1), QString("PDTR1607-7-I21-01:ARR%1:ArrayData").arg(i+1), this));
		thresholdControls_.append(new AMPVControl(QString("Threshold %1").arg(i+1), QString("PDTR1607-7-I21-01:C%1_SCA4_THRESHOLD_RBV").arg(i+1), QString("PDTR1607-7-I21-01:C%1_SCA4_THRESHOLD").arg(i+1), QString(), this, 0.5));
		icrControls_.append(new AMReadOnlyPVControl(QString("ICR %1").arg(i+1), QString("PDTR1607-7-I21-01:C%1_SCA4:Value_RBV").arg(i+1), this));
	}

	initializationControl_ = new AMSinglePVControl("Initialization", "PDTR1607-7-I21-01:Acquire", this, 0.1);
	eraseControl_ = new AMSinglePVControl("EraseControl", "PDTR1607-7-I21-01:ERASE", this, 0.5);
	updateControl_ = new AMSinglePVControl("UpdateControl", "PDTR1607-7-I21-01:UPDATE", this, 0.5);
	statusMessageControl_ = new AMReadOnlyPVControl("StatusMessage", "PDTR1607-7-I21-01:StatusMessage_RBV", this);
	currentFrameCountControl_ = new AMReadOnlyPVControl("CurrentFrame", "PDTR1607-7-I21-01:ArrayCounter_RBV", this);
	framesPerAcquisitionControl_ = new AMPVControl("FramesPerAcquisition", "PDTR1607-7-I21-01:NumImages_RBV", "PDTR1607-7-I21-01:NumImages", QString(), this, 0.5);

	makeConnections();
}

BioXASMainInboard32ElementGeDetector::~BioXASMainInboard32ElementGeDetector()
{

}

bool BioXASMainInboard32ElementGeDetector::canEnableElement(int index) const
{
	// Elements 3, 5, 11, 16, 22, and 27 (start 1) are disabled for this detector.
	return (index != 2 && index != 4 && index != 10 && index != 15 && index != 21 && index != 26);
}
