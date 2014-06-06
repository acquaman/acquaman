#ifndef AMCRASHMONITORSUPPORT_H
#define AMCRASHMONITORSUPPORT_H

#include <QFile>

class AMCrashMonitorSupport
{
public:
	static AMCrashMonitorSupport* s();

	qint64 crashMonitorPID() const;
	QFile* errorFile() const;

	void setErrorFile(QFile *errorFile);

	void monitor();
	void report();

protected:
	AMCrashMonitorSupport();

	qint64 crashMonitorPID_;
	QFile *errorFile_;

private:
	static AMCrashMonitorSupport *crashMonitorSupportInstance_;
};

void handle_signal(int signum);

#endif // AMCRASHMONITORSUPPORT_H
