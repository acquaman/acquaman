/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMErrorMonitor.h"

#include <QSystemTrayIcon>
#include <QMutableMapIterator>
#include <QDebug>
#include <QMutexLocker>
#include <QReadLocker>
#include <QWriteLocker>
#include <QThread>
#include <QApplication>

AMErrorMon* AMErrorMon::instance_ = 0;
QMutex AMErrorMon::instanceMutex_;

AMErrorMon::AMErrorMon() : QObject() {
	qRegisterMetaType<AMErrorReport>("AMErrorReport");
	sicon_ = new QSystemTrayIcon(QIcon(":/utilities-system-monitor.png"), this);
	sicon_->show();


	// don't display debug notifications by default:
	debugEnabled_ = false;

	// Set up our signal-based forwarding system to be thread-safe:
	connect(this, SIGNAL(reportFF(AMErrorReport)), this, SLOT(reportI(AMErrorReport)));
}


// Subscribe to all errors from object 'originator'
void AMErrorMon::subscribeToObjectI(const QObject* originator, QObject* notifyMe, const char* errorSlot) {
	/// \test Unit test insertions
	QWriteLocker l(&subsMutex_);
	objectSubs_.insertMulti(originator, QPair<QObject*,QString>(notifyMe, errorSlot));

}

// Subscribe to all errors from this class:
void AMErrorMon::subscribeToClassI(const QString& className, QObject* notifyMe, const char* errorSlot) {
	/// \test Unit test insertions
	QWriteLocker l(&subsMutex_);
	classSubs_.insertMulti(className, QPair<QObject*,QString>(notifyMe,errorSlot));
}

// Subscribe to all errors that have code 'errorCode'
void AMErrorMon::subscribeToCodeI(int errorCode, QObject* notifyMe, const char* errorSlot) {
	/// \test Unit test insertions
	QWriteLocker l(&subsMutex_);
	codeSubs_.insertMulti(errorCode, QPair<QObject*,QString>(notifyMe,errorSlot));
}

// Unsubscribe <notifyMe, errorSlot> from everything.  If errorSlot==0, unsubscribes all of notifyMe's subscriptions.
void AMErrorMon::unsubscribeI(QObject* notifyMe, const char* errorSlot) {

	QWriteLocker l(&subsMutex_);
	/// \test unit test removals (all cases)

	// Iterate through all object subscriptions
	QMutableMapIterator<const QObject*, QPair<QObject*,QString> > i(objectSubs_);
	while(i.hasNext()) {
		i.next();
		if( (notifyMe==i.value().first) && (errorSlot==0 || errorSlot==i.value().second) )
			i.remove();
	}

	// Iterate through all className subscriptions:
	QMutableMapIterator<QString, QPair<QObject*,QString> > i2(classSubs_);
	while(i2.hasNext()) {
		if( (notifyMe==i2.value().first) && (errorSlot==0 || errorSlot==i2.value().second) )
			i2.remove();
	}

	// Iterate through all errorCode subscriptions:
	QMutableMapIterator<int, QPair<QObject*,QString> > i3(codeSubs_);
	while(i3.hasNext()) {
		if( (notifyMe==i3.value().first) && (errorSlot==0 || errorSlot==i3.value().second) )
			i3.remove();
	}

}



// Handle error reports.
void AMErrorMon::reportI(const AMErrorReport& e) {

	QString className;
	if(e.source && e.source->metaObject())
		className = e.source->metaObject()->className();

	QString reportMessage = QString("in [%1]: %2 (code %3)").arg(className).arg(e.description).arg(e.errorCode);


	// Chapter 0: If we're in debug mode, throw a qDebug() for everything:
	if(debugEnabled_) {
		switch(e.level) {
		case AMErrorReport::Debug:
			qDebug() << "Debug:" << reportMessage;
			break;
		case AMErrorReport::Information:
			qDebug() << "Information:" << reportMessage;
			break;
		case AMErrorReport::Alert:
			qDebug() << "Alert:" << reportMessage;
			break;
		case AMErrorReport::Serious:
			qDebug() << "Serious:" << reportMessage;
			break;
		}
	}


	// Chapter 1: Emit signals, as long as it's not a debug message and debug is disabled.

	if( e.level == AMErrorReport::Debug && !debugEnabled_)
		; // do nothing
	else
		emit error(e);

	switch(e.level) {
	case AMErrorReport::Information:
		emit information(e);
		sicon_->showMessage(QString("Information from %1: (%2)").arg(className).arg(e.errorCode), e.description, QSystemTrayIcon::Information, 5000);
		break;
	case AMErrorReport::Alert:
		emit alert(e);
		sicon_->showMessage(QString("Alert from %1: (%2)").arg(className).arg(e.errorCode), e.description, QSystemTrayIcon::Warning, 5000);
		break;
	case AMErrorReport::Serious:
		emit serious(e);
		sicon_->showMessage(QString("Serious Error in %1: (%2)").arg(className).arg(e.errorCode), e.description, QSystemTrayIcon::Critical, 5000);
		break;
	case AMErrorReport::Debug:
		if(debugEnabled_) {
			emit debug(e);
			sicon_->showMessage(QString("Debug message from %1: (%2)").arg(className).arg(e.errorCode), e.description, QSystemTrayIcon::Information, 5000);
		}
		break;
	}

	// Chapter 2: go through various subscriptions:
	QReadLocker l(&subsMutex_);


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
	QPair<QObject*, QString> target;
	foreach(target, targets)
		target.first->metaObject()->invokeMethod( target.first, target.second.toAscii().data(), Q_ARG(AMErrorReport, e));


}

AMErrorMon * AMErrorMon::mon() {
	QMutexLocker ml(&instanceMutex_);

	if(!instance_) {
		instance_ = new AMErrorMon();
		if(QThread::currentThread() != QApplication::instance()->thread())
			instance_->moveToThread(QApplication::instance()->thread());
	}
	return instance_;
}
