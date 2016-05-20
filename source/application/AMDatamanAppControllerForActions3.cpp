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


#include "AMDatamanAppControllerForActions3.h"

#include <QStringBuilder>
#include <QFileInfo>
#include <QFile>
#include <QDateTime>
#include <QApplication>

#include "dataman/database/AMDbObjectSupport.h"

#include "actions3/AMLoopActionInfo3.h"
#include "actions3/AMTimeoutLoopActionInfo.h"
#include "actions3/actions/AMNumberChangeAction.h"
#include "actions3/AMActionInfo3.h"
#include "actions3/AMActionLog3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/actions/AMScanAction.h"
#include "actions3/actions/AMSamplePlatePre2013MoveAction.h"
#include "actions3/actions/AMSampleMoveActionInfo.h"
#include "actions3/actions/AMDetectorInitializeActionInfo.h"
#include "actions3/actions/AMDetectorWaitForAcquisitionStateActionInfo.h"
#include "actions3/actions/AMDetectorAcquisitionActionInfo.h"
#include "actions3/actions/AMDetectorTriggerActionInfo.h"
#include "actions3/actions/AMDetectorDwellTimeActionInfo.h"
#include "actions3/actions/AMDetectorReadActionInfo.h"
#include "actions3/actions/AMDetectorCleanupActionInfo.h"
#include "actions3/actions/AMAxisStartedActionInfo.h"
#include "actions3/actions/AMAxisFinishedActionInfo.h"
#include "actions3/actions/AMAxisValueFinishedActionInfo.h"
#include "actions3/actions/AMControlWaitActionInfo.h"
#include "actions3/actions/AMWaitActionInfo.h"
#include "actions3/actions/AMChangeToleranceActionInfo.h"
#include "actions3/actions/AMDetectorSetDarkCurrentValueActionInfo.h"
#include "actions3/actions/AMDetectorSetDarkCurrentTimeActionInfo.h"
#include "actions3/actions/AMDetectorSetDarkCurrentValidStateActionInfo.h"
#include "actions3/actions/AMDetectorSetLastMeasurementAsDarkCurrentActionInfo.h"

#include "util/AMErrorMonitor.h"

#include "dataman/databaseUpgrades/AMDbUpgrade1Pt1.h"
#include "dataman/databaseUpgrades/AMDbUpgrade1Pt2.h"

AMDatamanAppControllerForActions3::AMDatamanAppControllerForActions3(QObject *parent) :
	AMDatamanAppController(parent)
{
}

AMDatamanAppControllerForActions3::~AMDatamanAppControllerForActions3(){}

bool AMDatamanAppControllerForActions3::startupCreateDatabases()
{
	if(!AMDatamanAppController::startupCreateDatabases())
		return false;

	QStringList applicationArguments = QApplication::instance()->arguments();
	bool forceActionsDb = false;
	bool forceScanActionsDb = false;
	QString forcedActionsDb;
	QString forcedScanActionsDb;
	for(int x = 0, size = applicationArguments.size(); x < size; x++){

		if(applicationArguments.at(x).contains("--forceActionsDb=")){
			forceActionsDb = true;
			forcedActionsDb = applicationArguments.at(x);
			forcedActionsDb.remove("--forceActionsDb=");
		}
		else if(applicationArguments.at(x).contains("--forceScanActionsDb=")){
			forceScanActionsDb = true;
			forcedScanActionsDb = applicationArguments.at(x);
			forcedScanActionsDb.remove("--forceScanActionsDb=");
		}
	}

	QFileInfo actionsDatabaseFileInfo(AMUserSettings::userDataFolder%"/actionsData.db");
	if((actionsDatabaseFileInfo.size() > 1000000) && !forceActionsDb){
		int actionsDbSize = actionsDatabaseFileInfo.size();
		QString backupSuffix = QDateTime::currentDateTime().toString("MMMddyyyy_hhmmss");
		QFile::rename(QString("%1/actionsData.db").arg(AMUserSettings::userDataFolder) , QString("%1/actionsData.db.bk.%2").arg(AMUserSettings::userDataFolder).arg(backupSuffix));
		AMErrorMon::alert(this, AMDATAMANAPPCONTROLLERFORACTIONS3_MOVED_LARGE_ACTIONSDB, QString("Detected the actions database size was %1, so moved it to a backup file with suffix %2").arg(actionsDbSize).arg(backupSuffix));
	}

	// Create the Actions database
	QString actionsDbConnectionName;
	if(!forceActionsDb || forcedActionsDb.isEmpty())
		actionsDbConnectionName = QString("%1/%2").arg(AMUserSettings::userDataFolder).arg("actionsData.db");
	else
		actionsDbConnectionName = QString("%1/%2").arg(AMUserSettings::userDataFolder).arg(forcedActionsDb);

	AMDatabase *dbActions = AMDatabase::createDatabase("actions",  actionsDbConnectionName);
	if(!dbActions)
		return false;

	QFileInfo scanActionsDatabaseFileInfo(AMUserSettings::userDataFolder%"/scanActionsData.db");
	if(scanActionsDatabaseFileInfo.size() > 1000000 && !forceScanActionsDb){
		int scanActionsDbSize = scanActionsDatabaseFileInfo.size();
		QString backupSuffix = QDateTime::currentDateTime().toString("MMMddyyyy_hhmmss");
		QFile::rename(QString("%1/scanActionsData.db").arg(AMUserSettings::userDataFolder) , QString("%1/scanActionsData.db.bk.%2").arg(AMUserSettings::userDataFolder).arg(backupSuffix));
		AMErrorMon::alert(this, AMDATAMANAPPCONTROLLERFORACTIONS3_MOVED_LARGE_SCANACTIONSDB, QString("Detected the scan actions database size was %1, so moved it to a backup file with suffix %2").arg(scanActionsDbSize).arg(backupSuffix));
	}

	// Create the ScanActions database
	QString scanActionsDbConnectionName;
	if(!forceScanActionsDb || forcedScanActionsDb.isEmpty())
		scanActionsDbConnectionName = QString("%1/%2").arg(AMUserSettings::userDataFolder).arg("scanActionsData.db");
	else
		scanActionsDbConnectionName = QString("%1/%2").arg(AMUserSettings::userDataFolder).arg(forcedScanActionsDb);

	AMDatabase *dbScanActions = AMDatabase::createDatabase("scanActions", scanActionsDbConnectionName );
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
	AMDbObjectSupport::s()->registerClass<AMTimeoutLoopActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMActionLog3>();
	AMDbObjectSupport::s()->registerClass<AMNumberChangeActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMControlMoveActionInfo3>();
	AMDbObjectSupport::s()->registerClass<AMScanActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMSamplePlatePre2013MoveActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMSampleMoveActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMDetectorInitializeActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMDetectorWaitForAcquisitionStateActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMDetectorAcquisitionActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMDetectorTriggerActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMDetectorDwellTimeActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMDetectorReadActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMDetectorCleanupActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMAxisStartedActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMAxisFinishedActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMAxisValueFinishedActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMWaitActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMChangeToleranceActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMControlWaitActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMDetectorSetDarkCurrentValueActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMDetectorSetDarkCurrentTimeActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMDetectorSetDarkCurrentValidStateActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMDetectorSetLastMeasurementAsDarkCurrentActionInfo>();

	return true;
}

void AMDatamanAppControllerForActions3::shutdown()
{
	AMDatamanAppController::shutdown();

	AMDatabase::deleteDatabase("actions");
	AMDatabase::deleteDatabase("scanActions");
}
