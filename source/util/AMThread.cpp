#include "AMThread.h"

#include <QApplication>

AMThread::AMThread(QObject *parent) :
	QThread(parent)
{
	workerObject_ = 0;
	initialThread_ = 0;
}

void AMThread::setWorkerObject(QObject *workerObject){
	workerObject_ = workerObject;
}

void AMThread::setInitialThread(QThread *initialThread){
	initialThread_ = initialThread;
}

void AMThread::onWorkerFinished(){
	emit workerFinished();
}

void AMThread::run(){
	AMThreadHelper *threadHelper = new AMThreadHelper();
	threadHelper->setWorkerObject(workerObject_);
	if(initialThread_)
		threadHelper->setInitialThread(initialThread_);
	connect(this, SIGNAL(workerFinished()), threadHelper, SLOT(pushObject()));

	exec();
}

AMThreadHelper::AMThreadHelper(QObject *parent) :
	QObject(parent)
{
	workerObject_ = 0;
	initialThread_ = 0;
}

void AMThreadHelper::setWorkerObject(QObject *workerObject){
	workerObject_ = workerObject;
}

void AMThreadHelper::setInitialThread(QThread *initialThread){
	initialThread_ = initialThread;
}

void AMThreadHelper::pushObject(){
	if(initialThread_)
		workerObject_->moveToThread(initialThread_);
	else
		workerObject_->moveToThread(QApplication::instance()->thread());
}
