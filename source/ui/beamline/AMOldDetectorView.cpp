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


#include "AMOldDetectorView.h"

 AMOldDetectorView::~AMOldDetectorView(){}
AMOldDetectorView::AMOldDetectorView(bool configureOnly, QWidget *parent) :
	QWidget(parent)
{
	configureOnly_ = configureOnly;
}

AMOldDetector* AMOldDetectorView::detector(){
	return 0;
}

AMOldDetectorInfo* AMOldDetectorView::configurationSettings() const{
	return 0;
}

bool AMOldDetectorView::setDetector(AMOldDetector *detector, bool configureOnly){
	Q_UNUSED(detector)
	Q_UNUSED(configureOnly)
	return false;
}

 AMBriefOldDetectorView::~AMBriefOldDetectorView(){}
AMBriefOldDetectorView::AMBriefOldDetectorView(bool configureOnly, QWidget *parent) :
		AMOldDetectorView(configureOnly, parent)
{
}

bool AMBriefOldDetectorView::setDetector(AMOldDetector *detector, bool configureOnly){
	Q_UNUSED(detector)
	Q_UNUSED(configureOnly)
	return false;
}

 AMDetailedOldDetectorView::~AMDetailedOldDetectorView(){}
AMDetailedOldDetectorView::AMDetailedOldDetectorView(bool configureOnly, QWidget *parent) :
		AMOldDetectorView(configureOnly, parent)
{
}

bool AMDetailedOldDetectorView::setDetector(AMOldDetector *detector, bool configureOnly){
	Q_UNUSED(detector)
	Q_UNUSED(configureOnly)
	return false;
}
