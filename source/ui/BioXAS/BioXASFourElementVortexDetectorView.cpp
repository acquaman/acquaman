#include "BioXASFourElementVortexDetectorView.h"

BioXASFourElementVortexDetectorView::BioXASFourElementVortexDetectorView(BioXASFourElementVortexDetector *detector, QWidget *parent)
	: BioXASXRFDetectorView(detector, parent)
{
	deadTimeViewFactor_ = 2;
}
