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


#include "AMDetectorView.h"

AMDetectorView::AMDetectorView(bool configureOnly, QWidget *parent) :
	QWidget(parent)
{
	configureOnly_ = configureOnly;
}

AMDetector* AMDetectorView::detector(){
	return 0;
}

AMDetectorInfo* AMDetectorView::configurationSettings() const{
	return 0;
}

bool AMDetectorView::setDetector(AMDetector *detector, bool configureOnly){
	Q_UNUSED(detector)
	Q_UNUSED(configureOnly)
	return false;
}

AMBriefDetectorView::AMBriefDetectorView(bool configureOnly, QWidget *parent) :
		AMDetectorView(configureOnly, parent)
{
}

bool AMBriefDetectorView::setDetector(AMDetector *detector, bool configureOnly){
	Q_UNUSED(detector)
	Q_UNUSED(configureOnly)
	return false;
}

AMDetailedDetectorView::AMDetailedDetectorView(bool configureOnly, QWidget *parent) :
		AMDetectorView(configureOnly, parent)
{
}

bool AMDetailedDetectorView::setDetector(AMDetector *detector, bool configureOnly){
	Q_UNUSED(detector)
	Q_UNUSED(configureOnly)
	return false;
}
