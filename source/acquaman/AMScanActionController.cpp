#include "AMScanActionController.h"

AMScanActionController::AMScanActionController(AMScanConfiguration *configuration, QObject *parent) :
	AMScanController(configuration, parent)
{
}

bool AMScanActionController::initializeImplementation(){
	return false;
}

bool AMScanActionController::startImplementation(){
	return false;
}

bool AMScanActionController::canPause(){
	return false;
}

void AMScanActionController::pauseImplementation(){
}

void AMScanActionController::resumeImplementation(){
}

void AMScanActionController::cancelImplementation(){
}
