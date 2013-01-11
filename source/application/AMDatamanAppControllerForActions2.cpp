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


#include "AMDatamanAppControllerForActions2.h"

#include "dataman/database/AMDbObjectSupport.h"

#include "actions2/AMActionInfo.h"
#include "actions2/AMActionLog.h"
#include "actions2/actions/AMWaitActionInfo.h"
#include "actions2/actions/AMControlMoveActionInfo.h"
#include "actions2/actions/AMScanControllerActionInfo.h"
#include "actions2/actions/AMChangeRunActionInfo.h"

AMDatamanAppControllerForActions2::AMDatamanAppControllerForActions2(QObject *parent) :
    AMDatamanAppController(parent)
{
}


bool AMDatamanAppControllerForActions2::startupRegisterDatabases()
{
	if(!AMDatamanAppController::startupRegisterDatabases())
		return false;

	AMDbObjectSupport::s()->registerClass<AMActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMActionLog>();
	AMDbObjectSupport::s()->registerClass<AMWaitActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMControlMoveActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMScanControllerActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMChangeRunActionInfo>();

	return true;
}
