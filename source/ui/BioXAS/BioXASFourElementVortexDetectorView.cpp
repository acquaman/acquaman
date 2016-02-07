#include "BioXASFourElementVortexDetectorView.h"

BioXASFourElementVortexDetectorView::BioXASFourElementVortexDetectorView(BioXASFourElementVortexDetector *detector, QWidget *parent)
	: AMXspress3XRFDetectorView(detector, parent)
{
	deadTimeViewFactor_ = 2;
}
