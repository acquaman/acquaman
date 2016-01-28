#include "AMXRFScanController.h"

#include "dataman/AMScan.h"

AMXRFScanController::AMXRFScanController(AMXRFScanConfiguration *configuration, QObject *parent)
	: AMScanController(configuration, parent)
{

}

AMXRFScanController::~AMXRFScanController()
{

}

void AMXRFScanController::onDetectorAcquisitionFinished()
{

}

void AMXRFScanController::onProgressUpdate()
{

}

void AMXRFScanController::onStatusChanged()
{

}

//bool AMXRFScanController::initializeImplementation()
//{
//	return false;
//}

//bool AMXRFScanController::startImplementation()
//{
//	return false;
//}

//void AMXRFScanController::cancelImplementation()
//{

//}

//void AMXRFScanController::stopImplementation(const QString &command)
//{
//	Q_UNUSED(command)
//}

void AMXRFScanController::saveData()
{

}
