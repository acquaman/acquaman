#include "MCPDetectorView.h"

MCPDetectorView::MCPDetectorView(MCPDetector *detector, QWidget *parent) :
	AMDetectorView(parent)
{
	hl_ = 0;
	fbk_ = 0;
	setDetector(detector);
}

AMDetector* MCPDetectorView::detector(){
	return detector_;
}

bool MCPDetectorView::setDetector(AMDetector *detector){
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

