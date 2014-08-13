#ifndef AMTHREAD_H
#define AMTHREAD_H

#include <QThread>

class AMThreadHelper;

class AMThread : public QThread
{
Q_OBJECT
public:
	/// Subclasses QThread to provide a thread that can return a worker object to the calling thread when a finished signal is emitted from the object
	AMThread(QObject *parent = 0);

	/// Sets the worker object that will be returned when the job is completed
	void setWorkerObject(QObject *workerObject);
	/// Sets the initial thread that the object will be returned to. If none is provided, the the QApplication::instance()->thread() will be the destination.
	void setInitialThread(QThread *initialThread);

signals:
	/// Emitted from this thread object to the helper object to actually push the worker object back
	void workerFinished();

public slots:
	/// Handles emitting the workerFinished() signal
	void onWorkerFinished();

protected:
	/// Actual thread execution, creates a helper object in this thread so it can push the worker back
	virtual void run();

protected:
	/// Pointer to the worker object
	QObject *workerObject_;
	/// Thread to return the worker object to
	QThread *initialThread_;
};

class AMThreadHelper : public QObject
{
Q_OBJECT
public:
	/// Helper class that acts as an object to do the pushing from the work thread back to the calling thread (the AMThread can't do this because it's constructor is called from the calling thread, not its own thread)
	AMThreadHelper(QObject *parent = 0);

	/// Sets the worker object that we want to push back
	void setWorkerObject(QObject *workerObject);
	/// Sets the thread we're going to the worker object back to. If none is provided, the the QApplication::instance()->thread() will be the destination.
	void setInitialThread(QThread *initialThread);

public slots:
	/// Actually pushes the object back to the initial thread
	void pushObject();

protected:
	/// The worker object we're going to push back after its job is done
	QObject *workerObject_;
	/// The thread we're going to push the worker object back to
	QThread *initialThread_;
};

#endif // AMTHREAD_H
