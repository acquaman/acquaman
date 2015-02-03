#include "BioXAS32ElementGeDetectorView.h"

BioXAS32ElementGeDetectorView::BioXAS32ElementGeDetectorView(BioXAS32ElementGeDetector *detector, QWidget *parent)
	: AMXRFBaseDetectorView(detector, parent)
{

}

void BioXAS32ElementGeDetectorView::buildDetectorView()
{
	// This is going to extend the base view, so we will simply call it's build method too.
	AMXRFBaseDetectorView::buildDetectorView();

	statusMessageLabel_ = new QLabel("");
	connect(bioXAS32ElementGeDetector(), SIGNAL(statusMessageChanged(QString)), statusMessageLabel_, SLOT(setText(QString)));

	rightLayout_->addWidget(statusMessageLabel_);
}
