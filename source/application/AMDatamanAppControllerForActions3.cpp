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


#include "AMDatamanAppControllerForActions3.h"

#include "dataman/database/AMDbObjectSupport.h"

#include "actions3/AMLoopActionInfo3.h"
#include "actions3/actions/AMNumberChangeAction.h"
#include "actions3/AMActionInfo3.h"
#include "actions3/AMActionLog3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/actions/AMScanAction.h"
#include "actions3/actions/AMSamplePlateMoveAction.h"
#include "actions3/actions/AMDetectorInitializeActionInfo.h"
#include "actions3/actions/AMDetectorAcquisitionActionInfo.h"
#include "actions3/actions/AMDetectorTriggerActionInfo.h"
#include "actions3/actions/AMDetectorDwellTimeActionInfo.h"
#include "actions3/actions/AMDetectorReadActionInfo.h"
#include "actions3/actions/AMDetectorCleanupActionInfo.h"
#include "actions3/actions/AMAxisStartedActionInfo.h"
#include "actions3/actions/AMAxisFinishedActionInfo.h"
#include "actions3/actions/AMAxisValueFinishedActionInfo.h"
#include "actions3/actions/AMTimedWaitActionInfo3.h"
#include "actions3/actions/AMControlWaitActionInfo.h"
#include "actions3/actions/AMDoDarkCurrentCorrectionActionInfo.h"
#include "actions3/actions/AMDoingDarkCurrentCorrectionActionInfo.h"

#include "dataman/AMDbUpgrade1Pt1.h"
#include "dataman/AMDbUpgrade1Pt2.h"

#include <QStringBuilder>

 AMDatamanAppControllerForActions3::~AMDatamanAppControllerForActions3(){}
AMDatamanAppControllerForActions3::AMDatamanAppControllerForActions3(QObject *parent) :
	AMDatamanAppController(parent)
{
	// Prepend the AM upgrade 1.1 to the list for the actions database
	AMDbUpgrade *am1Pt1UserDb = new AMDbUpgrade1Pt1("actions", this);
	prependDatabaseUpgrade(am1Pt1UserDb);

	// Append the AM upgrade 1.2 to the list for the actions database
	AMDbUpgrade *am1Pt2UserDb = new AMDbUpgrade1Pt2("actions", this);
	appendDatabaseUpgrade(am1Pt2UserDb);
}

bool AMDatamanAppControllerForActions3::startupCreateDatabases()
{
	if(!AMDatamanAppController::startupCreateDatabases())
		return false;

	// Create the Actions database
	AMDatabase *dbActions = AMDatabase::createDatabase("actions", AMUserSettings::userDataFolder%"/actionsData.db" );
	if(!dbActions)
		return false;

	// Create the ScanActions database
	AMDatabase *dbScanActions = AMDatabase::createDatabase("scanActions", AMUserSettings::userDataFolder%"/scanActionsData.db" );
	if(!dbScanActions)
		return false;

	return true;
}

bool AMDatamanAppControllerForActions3::startupRegisterDatabases()
{
	if(!AMDatamanAppController::startupRegisterDatabases())
		return false;

	// Grab the Action database
	AMDatabase *dbActions = AMDatabase::database("actions");
	if(!dbActions)
		return false;

	// Grab the Scan Action database
	AMDatabase *dbScanActions = AMDatabase::database("scanActions");
	if(!dbScanActions)
		return false;

	// Register the Actions database
	if(!AMDbObjectSupport::s()->registerDatabase(dbActions)) {
		return false;
	}

	// Register the Scan Actions database
	if(!AMDbObjectSupport::s()->registerDatabase(dbScanActions)) {
		return false;
	}

	AMDbObjectSupport::s()->registerClass<AMActionInfo3>();
	AMDbObjectSupport::s()->registerClass<AMListActionInfo3>();
	AMDbObjectSupport::s()->registerClass<AMSequentialListActionInfo3>();
	AMDbObjectSupport::s()->registerClass<AMParallelListActionInfo3>();
	AMDbObjectSupport::s()->registerClass<AMLoopActionInfo3>();
	AMDbObjectSupport::s()->registerClass<AMActionLog3>();
	AMDbObjectSupport::s()->registerClass<AMNumberChangeActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMControlMoveActionInfo3>();
	AMDbObjectSupport::s()->registerClass<AMScanActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMSamplePlateMoveActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMDetectorInitializeActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMDetectorAcquisitionActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMDetectorTriggerActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMDetectorDwellTimeActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMDetectorReadActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMDetectorCleanupActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMAxisStartedActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMAxisFinishedActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMAxisValueFinishedActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMTimedWaitActionInfo3>();

    AMDbObjectSupport::s()->registerClass<AMControlWaitActionInfo>();

    AMDbObjectSupport::s()->registerClass<AMDoDarkCurrentCorrectionActionInfo>();
    AMDbObjectSupport::s()->registerClass<AMDoingDarkCurrentCorrectionActionInfo>();

	return true;
}
