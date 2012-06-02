/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMSingleControlDetectorView.h"

AMSingleControlBriefDetectorView::AMSingleControlBriefDetectorView(AMSingleControlDetector *detector, QWidget *parent) :
		AMBriefDetectorView(false, parent)
{
	hl_ = 0;
	fbk_ = 0;
	detector_ = 0;
	configurationSettings_ = 0;
	setDetector(detector);
}

AMDetector* AMSingleControlBriefDetectorView::detector(){
	return detector_;
}

AMDetectorInfo* AMSingleControlBriefDetectorView::configurationSettings() const{
	return configurationSettings_;
}

bool AMSingleControlBriefDetectorView::setDetector(AMDetector *detector, bool configureOnly){
	if(!detector)
		return false;
	if(configurationSettings_)
		configurationSettings_->deleteLater();
	detector_ = static_cast<AMSingleControlDetector*>(detector);
	// Just created this, now I'm responsible
	configurationSettings_ = detector_->toNewInfo();
	configureOnly_ = configureOnly;
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


//////////////////////////////////////////////

AMSingleReadOnlyControlBriefDetectorView::AMSingleReadOnlyControlBriefDetectorView(AMSingleReadOnlyControlDetector *detector, QWidget *parent) :
		AMBriefDetectorView(false, parent)
{
	hl_ = 0;
	fbk_ = 0;
	detector_ = 0;
	configurationSettings_ = 0;
	setDetector(detector);
}

AMDetector* AMSingleReadOnlyControlBriefDetectorView::detector(){
	return detector_;
}

AMDetectorInfo* AMSingleReadOnlyControlBriefDetectorView::configurationSettings() const{
	return configurationSettings_;
}

bool AMSingleReadOnlyControlBriefDetectorView::setDetector(AMDetector *detector, bool configureOnly){
	if(!detector)
		return false;
	if(configurationSettings_)
		configurationSettings_->deleteLater();
	detector_ = static_cast<AMSingleReadOnlyControlDetector*>(detector);
	// Just created this, now I'm responsible
	configurationSettings_ = detector_->toNewInfo();
	configureOnly_ = configureOnly;
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

