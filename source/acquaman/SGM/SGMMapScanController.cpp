#include "SGMMapScanController.h"

#include "beamline/AMBeamline.h"
#include "beamline/CLS/CLSAMDSScalerChannelDetector.h"

#include "source/ClientRequest/AMDSClientDataRequest.h"
#include "source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"
#include "source/DataHolder/AMDSGenericFlatArrayDataHolder.h"
#include "source/DataHolder/AMDSSpectralDataHolder.h"
#include "source/DataHolder/AMDSScalarDataHolder.h"

SGMMapScanController::SGMMapScanController(SGMMapScanConfiguration *configuration, QObject *parent)
	: AMGenericContinuousScanController(configuration, parent)
{

}

SGMMapScanController::~SGMMapScanController()
{

}

void SGMMapScanController::onAxisFinished()
{
	// PUT CODE IN HERE DAVE
}

void SGMMapScanController::fillDataMaps(AMAgnosticDataAPIDataAvailableMessage *message)
{
	// PLACE HOLDER CODE FOR DAVE
	if(message->uniqueID() == "GratingEncoderFeedback")
		metaDataMap_.insert(message->uniqueID(), message->value("DetectorData").toList().at(0).toDouble());
	else{
		intptr_t dataRequestPointer = message->detectorDataAsAMDS();
		void *dataRequestVoidPointer = (void*)dataRequestPointer;
		AMDSClientDataRequest *dataRequest = static_cast<AMDSClientDataRequest*>(dataRequestVoidPointer);
		clientDataRequestMap_.insert(dataRequest->bufferName(), dataRequest);
	}
}

AMAction3 *SGMMapScanController::createInitializationActions()
{
	return 0;
}

AMAction3 *SGMMapScanController::createCleanupActions()
{
	return 0;
}

