#include "BioXAS32ElementGeDetectorView.h"

BioXAS32ElementGeDetectorView::BioXAS32ElementGeDetectorView(BioXAS32ElementGeDetector *detector, QWidget *parent)
	: AMXspress3XRFDetectorView(detector, parent)
{
	deadTimeViewFactor_ = 8;
}
