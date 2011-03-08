#include "AMDetectorSetView.h"

AMDetectorSetView::AMDetectorSetView(AMDetectorSet *viewSet, bool configureOnly, QWidget *parent) :
		QGroupBox(parent)
{
	viewSet_ = viewSet;
	configureOnly_ = configureOnly;
	setTitle(viewSet->name());
	vl_ = new QVBoxLayout();
	//AMControlEditor *tmpCE;
	//AMControl *tmpCtrl;
	AMDetectorView *tmpDV;
	AMDetector *tmpD;
	for(int x = 0; x < viewSet_->count(); x++){
		tmpD = viewSet_->at(x);
		//tmpCE = new AMControlEditor(tmpCtrl, 0, false, configureOnly_);
		tmpDV = AMDetectorViewSupport::createDetectorView(tmpD);
		vl_->addWidget(tmpDV);
		detectorBoxes_.append(tmpDV);
		//connect(tmpCE, SIGNAL(setpointRequested(double)), this, SLOT(onConfigurationValueChanged()));
	}

	hl_ = new QHBoxLayout(this);
	hl_->addLayout(vl_);
	setLayout(hl_);
	setFixedSize(300, 200);

	connect(viewSet_, SIGNAL(detectorAdded(int)), this, SLOT(onDetectorAddedToSet(int)));

	//connect(viewSet_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(onControlSetValuesChanged(AMControlInfoList)));
}

void AMDetectorSetView::onDetectorAddedToSet(int index){
	AMDetectorView *tmpDV;
	AMDetector *tmpD;
	tmpD = viewSet_->at(index);
	tmpDV = AMDetectorViewSupport::createDetectorView(tmpD);
	vl_->insertWidget(index, tmpDV);
	detectorBoxes_.insert(index, tmpDV);
}

