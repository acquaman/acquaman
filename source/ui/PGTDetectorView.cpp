#include "PGTDetectorView.h"

PGTDetectorView::PGTDetectorView(PGTDetector *detector, QWidget *parent) :
	AMDetectorView(parent)
{
	controlLabel_ = new QLabel("PGTDetectorView");
	hl_ = new QHBoxLayout();
	hl_->addWidget(controlLabel_);
	setLayout(hl_);
	//detector_ = detector;
	setDetector(detector);
}

AMDetector* PGTDetectorView::detector(){
	return detector_;
}

bool PGTDetectorView::setDetector(AMDetector *detector){
	/*
	PGTDetector *d = qobject_cast<PGTDetector*>(detector);
	if(!d)
		return false;
	*/
	if(!detector)
		return false;
	detector_ = static_cast<PGTDetector*>(detector);
	controlLabel_->setText(detector_->name());
	return true;
}

