#include "MCPDetectorView.h"

MCPBriefDetectorView::MCPBriefDetectorView(MCPDetector *detector, QWidget *parent) :
	AMBriefDetectorView(parent)
{
	hl_ = 0;
	fbk_ = 0;
	setDetector(detector);
}

AMDetector* MCPBriefDetectorView::detector(){
	return detector_;
}

bool MCPBriefDetectorView::setDetector(AMDetector *detector){
	/*
	MCPDetector *d = qobject_cast<MCPDetector*>(detector);
	if(!d)
		return false;
	*/
	if(!detector)
		return false;
	detector_ = static_cast<MCPDetector*>(detector);
	if(!hl_){
		hl_ = new QHBoxLayout();
		setLayout(hl_);
	}
	if(fbk_){
		hl_->removeWidget(fbk_);
		delete fbk_;
		fbk_ = 0;
	}
	fbk_ = new AMControlEditor(detector_->readingCtrl(), 0, true);
	hl_->addWidget(fbk_);
	return true;
}


MCPDetailedDetectorView::MCPDetailedDetectorView(MCPDetector *detector, QWidget *parent) :
	AMDetailedDetectorView(parent)
{
	fl_ = 0;
	hvSetpointLabel_ = 0;
	setDetector(detector);
}

AMDetector* MCPDetailedDetectorView::detector(){
	return detector_;
}

bool MCPDetailedDetectorView::setDetector(AMDetector *detector){
	/*
	MCPDetector *d = qobject_cast<MCPDetector*>(detector);
	if(!d)
		return false;
	*/
	if(!detector)
		return false;
	detector_ = static_cast<MCPDetector*>(detector);
	if(!fl_){
		fl_ = new QFormLayout();
		setLayout(fl_);
	}
	if(hvSetpointLabel_){
		fl_->removeWidget(hvSetpointLabel_);
		delete hvSetpointLabel_;
		hvSetpointLabel_ = 0;
	}
	hvSetpointLabel_ = new QLabel(QString("%1").arg(detector_->hvSetpoint()));
	fl_->addRow("HV", hvSetpointLabel_);
	return true;
}
