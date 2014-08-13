#ifndef AMTHREAD_H
#define AMTHREAD_H

#include <QThread>

class AMThreadHelper;

class AMThread : public QThread
{
Q_OBJECT
public:
	AMThread(QObject *parent = 0);

	void setWorkerObject(QObject *workerObject);
	void setInitialThread(QThread *initialThread);

signals:
	void workerFinished();

public slots:
	void onWorkerFinished();

protected:
	virtual void run();

protected:
	QObject *workerObject_;
	QThread *initialThread_;
};

class AMThreadHelper : public QObject
{
Q_OBJECT
public:
	AMThreadHelper(QObject *parent = 0);

	void setWorkerObject(QObject *workerObject);
	void setInitialThread(QThread *initialThread);

public slots:
	void pushObject();

protected:
	QObject *workerObject_;
	QThread *initialThread_;
};

#endif // AMTHREAD_H
