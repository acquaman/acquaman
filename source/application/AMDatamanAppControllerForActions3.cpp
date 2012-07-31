#include "AMDatamanAppControllerForActions3.h"

#include "dataman/database/AMDbObjectSupport.h"

#include "actions3/AMLoopActionInfo3.h"
#include "actions3/actions/AMNumberChangeAction.h"
#include "actions3/AMActionInfo3.h"
#include "actions3/AMActionLog3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/actions/AMScanAction.h"
#include "actions3/actions/AMSamplePlateMoveAction.h"

#include <QStringBuilder>

AMDatamanAppControllerForActions3::AMDatamanAppControllerForActions3(QObject *parent) :
    AMDatamanAppController(parent)
{
}

bool AMDatamanAppControllerForActions3::startupCreateDatabases()
{
	if(!AMDatamanAppController::startupCreateDatabases())
		return false;

	// Create the Actions database
	AMDatabase *dbActions = AMDatabase::createDatabase("actions", AMUserSettings::userDataFolder%"/actionsData.db" );
	if(!dbActions)
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

	// Register the Actions database
	if(!AMDbObjectSupport::s()->registerDatabase(dbActions)) {
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

	return true;
}
