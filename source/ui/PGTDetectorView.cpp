#include "PGTDetectorView.h"

PGTBriefDetectorView::PGTBriefDetectorView(PGTDetector *detector, QWidget *parent) :
	AMBriefDetectorView(parent)
{
	controlLabel_ = new QLabel("PGTDetectorView");
	hl_ = new QHBoxLayout();
	hl_->addWidget(controlLabel_);
	setLayout(hl_);
	//detector_ = detector;
	setDetector(detector);
}

AMDetector* PGTBriefDetectorView::detector(){
	return detector_;
}

bool PGTBriefDetectorView::setDetector(AMDetector *detector){
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


PGTDetailedDetectorView::PGTDetailedDetectorView(PGTDetector *detector, QWidget *parent) :
	AMDetailedDetectorView(parent)
{
	fl_ = new QFormLayout();
	hvSetpointLabel_ = new QLabel();
	integrationModeLabel_ = new QLabel();
	integrationTimeLabel_ = new QLabel();
	fl_->addRow("HV", hvSetpointLabel_);
	fl_->addRow("Mode", integrationModeLabel_);
	fl_->addRow("Time", integrationTimeLabel_);
	setLayout(fl_);
	setDetector(detector);
}

AMDetector* PGTDetailedDetectorView::detector(){
	return detector_;
}

bool PGTDetailedDetectorView::setDetector(AMDetector *detector){
	/*
	PGTDetector *d = qobject_cast<PGTDetector*>(detector);
	if(!d)
		return false;
	*/
	if(!detector)
		return false;
	detector_ = static_cast<PGTDetector*>(detector);
	hvSetpointLabel_->setText(QString("%1").arg(detector_->hvSetpoint()));
	integrationModeLabel_->setText(detector_->integrationMode());
	integrationTimeLabel_->setText(QString("%1").arg(detector_->integrationTime()));
	return true;
}
