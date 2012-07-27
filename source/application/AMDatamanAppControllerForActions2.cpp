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
