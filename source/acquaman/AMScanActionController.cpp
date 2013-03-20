#include "AMScanActionController.h"

AMScanActionController::AMScanActionController(AMScanConfiguration *configuration, QObject *parent) :
	AMScanController(configuration, parent)
{
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

#include "acquaman/AMAgnosticDataAPI.h"
bool AMScanActionController::event(QEvent *e){
	if(e->type() == (QEvent::Type)AMAgnosticDataAPIDefinitions::MessageEvent){
		AMAgnosticDataAPIMessage message = ((AMAgnositicDataEvent*)e)->message_;

		qDebug() << "Just heard a message " << message.toJSON();

		e->accept();
		return true;
	}
	else
		return AMScanController::event(e);
}
