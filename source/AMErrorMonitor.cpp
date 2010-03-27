#include "AMErrorMonitor.h"

#include <QSystemTrayIcon>

AMErrorMon* AMErrorMon::instance_ = 0;

AMErrorMon::AMErrorMon() : QObject() {
        qRegisterMetaType<AMErrorReport>("AMErrorReport");
	sicon_ = new QSystemTrayIcon();
	sicon_->show();
}


/// Subscribe to all errors from object 'originator'
void AMErrorMon::subscribeToObjectI(QObject* originator, QObject* notifyMe, const char* errorSlot) {
	// Remember that this

}

/// Subscribe to all errors from this class:
void AMErrorMon::subscribeToClassI(const QString& className, QObject* notifyMe, const char* errorSlot) {

}

/// Subscribe to all errors that have code 'errorCode'
void AMErrorMon::subscribeToCodeI(int errorCode, QObject* notifyMe, const char* errorSlot) {

}

/// Unsubscribe from everything:
void AMErrorMon::unsubscribeI(QObject* notifyMe) {

}



/// Report an error:
void AMErrorMon::reportI(const AMErrorReport& e) {
	// Chapter 1: Emit signals:
	emit error(e);

	QString className = "[]";

	switch(e.level) {
        case AMErrorReport::Information: emit information(e);
		break;
        case AMErrorReport::Alert: emit alert(e);
		break;
        case AMErrorReport::Serious:
		emit serious(e);
		if(e.source)
			className = e.source->metaObject()->className();
		sicon_->showMessage(QString("Error in %1: (%2)").arg(className).arg(e.errorCode), e.description, QSystemTrayIcon::Critical, 5000);
		break;
        case AMErrorReport::Debug: emit debug(e);
		break;
	}

	// Chapter 2: go through various subscriptions:


	// This looks complicated.  It's a list of object pointers and corresponding slotNames that need to be called.  We fill it using the subscription records.
	QList<QPair<QObject*, QString> > targets;

	// Object subscriptions:
	targets = objectSubs_.values(e.source);
	// Classname subscriptions:
	if(e.source)
		targets.append( classSubs_.values(e.source->metaObject()->className()) );
	// Errorcode subscriptions:
	targets.append( codeSubs_.values(e.errorCode) );


	// Invoke the slot in each object (We use this to be thread-safe... The notification is delivered just like a slot call.
	// If the object is in the same thread, it's delivered right away.  If in different threads, it's queued into the event loop.
	for(int i=0; i< targets.count(); i++) {
		QPair<QObject*, QString>& target = targets[i];
                target.first->metaObject()->invokeMethod( target.first, target.second.toAscii().data(), Q_ARG(AMErrorReport, e));
	}
	

}
