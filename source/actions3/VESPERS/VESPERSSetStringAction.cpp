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


#include "VESPERSSetStringAction.h"

#include "util/AMErrorMonitor.h"
#include "application/VESPERS/VESPERS.h"

 VESPERSSetStringAction::~VESPERSSetStringAction(){}
VESPERSSetStringAction::VESPERSSetStringAction(VESPERSSetStringActionInfo *info, AMControl *control, QObject *parent)
	: AMAction3(info, parent)
{
	if (control)
		control_ = control;
	else
		control_ = 0;
}

VESPERSSetStringAction::VESPERSSetStringAction(const VESPERSSetStringAction &other)
	: AMAction3(other)
{
	control_ = other.control();
}

void VESPERSSetStringAction::startImplementation()
{
	// Must have a control, and it must be able to move.
	if(!control_) {
		AMErrorMon::alert(this,
						  VESPERSSETSTRINGACTION_NOCONTROL,
						  QString("There was an error setting the string to the control, because the control was not found. Please report this problem to the Acquaman developers."));
		setFailed();
		return;
	}

	AMProcessVariable *pv = ((AMPVControl *)(control_))->writePV();
	connect(pv, SIGNAL(valueChanged()), this, SLOT(onValueChanged()));
	VESPERS::stringToPV(pv, ((VESPERSSetStringActionInfo *)info())->text());
}

void VESPERSSetStringAction::onValueChanged()
{
	const AMProcessVariable *pv = ((AMPVControl *)(control_))->readPV();
	QString string = VESPERS::pvToString(pv);
	QString text = ((VESPERSSetStringActionInfo *)info())->text();

	if (string == text){

		AMErrorMon::debug(this, VESPERSSETSTRINGACTION_SUCCEEDED, QString("[SUCCEEDED] Set string.  Desired: %1 Result: %2.").arg(text).arg(string));
		setSucceeded();
	}

	else{

		AMErrorMon::debug(this, VESPERSSETSTRINGACTION_FAILED, QString("[FAILED] Set string.  Desired: %1 Result: %2.").arg(text).arg(string));
		setFailed();
	}
}
