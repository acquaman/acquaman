#include "BioXAS32ElementGeDetector.h"

BioXAS32ElementGeDetector::BioXAS32ElementGeDetector(const QString &name, const QString &description, AMPVControl *triggerControl, BioXASZebraPulseControl *pulseControl, QObject *parent)
	: AMXspress3XRFDetector(name, description, parent)
{
	// Stuff required by AMXRFDetector.

	pulseControl_ = pulseControl;
	acquireControl_ = triggerControl;
	acquireTimeControl_ = pulseControl_->pulseWidthSecondsControl();
	elapsedTimeControl_ = 0;
}

BioXAS32ElementGeDetector::~BioXAS32ElementGeDetector()
{

}
#include <QDebug>
bool BioXAS32ElementGeDetector::setAcquisitionTime(double seconds)
{
	qDebug() << "Detector is connected:" << (isConnected() ? "Yes" : "No");

	if(!isConnected())
		return false;

	pulseControl_->setPulseWidthValueSeconds(seconds);

	return true;
}
