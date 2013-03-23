#include "AMScanActionController.h"

AMScanActionController::AMScanActionController(AMScanConfiguration *configuration, QObject *parent) :
	AMScanController(configuration, parent)
{
}

#include "actions3/AMActionRunner3.h"

#include "acquaman/AMAgnosticDataAPI.h"
bool AMScanActionController::startImplementation(){
	qDebug() << "Trying to call startImplementation in AMScanActionController";
	AMAgnosticDataMessageQEventHandler *scanActionMessager = new AMAgnosticDataMessageQEventHandler();
	AMAgnosticDataAPISupport::registerHandler("ScanActions", scanActionMessager);
	scanActionMessager->addReceiver(this);

	AMActionRunner3::scanActionRunner()->setQueuePaused(false);
	setStarted();
	return true;
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
