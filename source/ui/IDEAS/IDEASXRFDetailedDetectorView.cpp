/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "IDEASXRFDetailedDetectorView.h"

#include "actions3/AMListAction3.h"

IDEASXRFDetailedDetectorView::IDEASXRFDetailedDetectorView(AMXRFDetector *detector, QWidget *parent)
	: AMXRFDetailedDetectorView(detector, parent)
{
	setMaximumHeight(885);
}

IDEASXRFDetailedDetectorView::~IDEASXRFDetailedDetectorView()
{
}

void IDEASXRFDetailedDetectorView::buildDetectorView()
{
	// This is going to extend the base view, so we will simply call it's build method too.
	AMXRFDetailedDetectorView::buildDetectorView();

	buildScanSaveViews();

}

void IDEASXRFDetailedDetectorView::buildScanSaveViews()
{
	deadTimeCheckButton_ = new QPushButton("Check Dead Time");

	connect(deadTimeCheckButton_, SIGNAL(clicked()), this, SLOT(onDeadTimeCheckButtonClicked()));

	rightLayout_->addWidget(deadTimeCheckButton_);
	rightLayout_->addStretch();
}

void IDEASXRFDetailedDetectorView::onDeadTimeCheckButtonClicked()
{
	if(detector_->isAcquiring())
		return;

	double requestedTime = detector_->acquisitionTime();

	AMListAction3 *deadTimeCheckActions = new AMListAction3(new AMListActionInfo3("Quick Deadtime Check", "Quick Deadtime Check"));
	deadTimeCheckActions->addSubAction(detector_->createSetAcquisitionTimeAction(0.1));
	deadTimeCheckActions->addSubAction(detector_->createAcquisitionAction(AMDetectorDefinitions::SingleRead));
	deadTimeCheckActions->addSubAction(detector_->createSetAcquisitionTimeAction(requestedTime));

	connect(deadTimeCheckActions, SIGNAL(cancelled()), deadTimeCheckActions, SLOT(deleteLater()));
	connect(deadTimeCheckActions, SIGNAL(failed()), deadTimeCheckActions, SLOT(deleteLater()));
	connect(deadTimeCheckActions, SIGNAL(succeeded()), deadTimeCheckActions, SLOT(deleteLater()));

	deadTimeCheckActions->start();
}
