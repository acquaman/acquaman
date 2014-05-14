#include "STAppController.h"

#include "beamline/AMProcessVariablePrivate.h"

STAppController::STAppController(QObject *parent) :
    QObject(parent)
{
    isStarting_ = true;
    isShuttingDown_ = false;

    model_ = new STModel(this);
}

STAppController::~STAppController()
{

}

bool STAppController::startup()
{
    setupUserInterface();

    isStarting_ = false;

    return true;
}

bool STAppController::isRunning()
{
    return (isStarting_ == false) && (isShuttingDown_ == false);
}

void STAppController::shutdown()
{
    isShuttingDown_ = true;

    AMProcessVariableSupport::shutdownChannelAccess();
}

void STAppController::setupUserInterface()
{
}
