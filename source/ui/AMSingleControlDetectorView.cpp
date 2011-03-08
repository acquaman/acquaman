#include "AMSingleControlDetectorView.h"

AMSingleControlDetectorView::AMSingleControlDetectorView(AMSingleControlDetector *detector, QWidget *parent) :
		AMDetectorView(parent)
{
	hl_ = 0;
	fbk_ = 0;
	setDetector(detector);
}

AMDetector* AMSingleControlDetectorView::detector(){
	return detector_;
}

bool AMSingleControlDetectorView::setDetector(AMDetector *detector){
	/*
	AMSingleControlDetector *d = qobject_cast<AMSingleControlDetector*>(detector);
	if(!d)
		return false;
		*/
	if(!detector)
		return false;
	detector_ = static_cast<AMSingleControlDetector*>(detector);
	if(!hl_){
		hl_ = new QHBoxLayout();
		setLayout(hl_);
	}
	if(fbk_){
		hl_->removeWidget(fbk_);
		delete fbk_;
		fbk_ = 0;
	}
	fbk_ = new AMControlEditor(detector_->control(), 0, true);
	hl_->addWidget(fbk_);
	return true;
}

