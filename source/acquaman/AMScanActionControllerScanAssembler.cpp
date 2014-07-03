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


#include "AMScanActionControllerScanAssembler.h"

AMScanActionControllerScanAssembler::~AMScanActionControllerScanAssembler(){}

AMScanActionControllerScanAssembler::AMScanActionControllerScanAssembler(QObject *parent) :
	QObject(parent)
{
	detectors_ = new AMDetectorSet(this);
	controls_ = new AMControlSet(this);
	actionTree_ = 0; //NULL
}

bool AMScanActionControllerScanAssembler::insertAxis(int index, AMControl *axisControl, AMScanAxis *scanAxis)
{
	if(!scanAxis)
		return false;
	if((axisControl && controls_->contains(axisControl->name())) || axes_.contains(scanAxis))
		return false;
	if(index < 0 || index > controls_->count())
		return false;
	if(controls_->count() != axes_.count())
		return false;

	if (axisControl)
		controls_->insert(index, axisControl, axisControl->name());

	else
		controls_->insert(index, axisControl, "");

	axes_.insert(index, scanAxis);

	return true;
}

bool AMScanActionControllerScanAssembler::appendAxis(AMControl *axisControl, AMScanAxis *scanAxis)
{
	return insertAxis(controls_->count(), axisControl, scanAxis);
}

bool AMScanActionControllerScanAssembler::addDetector(AMDetector *detector)
{
	return detectors_->addDetector(detector);
}

bool AMScanActionControllerScanAssembler::generateActionTree()
{
	if (generateActionTreeImplmentation()){

		emit actionTreeGenerated(actionTree_);
		return true;
	}

	return false;
}
