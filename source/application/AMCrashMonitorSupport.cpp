#include "AMCrashMonitorSupport.h"

#include <signal.h>
#include <execinfo.h>

#include <QApplication>
#include <QFileInfo>
#include <QProcess>
#include <QDir>

#include <QDebug>

qint64 globalCrashMonitorPID;
QFile *globalErrorFile;

AMCrashMonitorSupport* AMCrashMonitorSupport::crashMonitorSupportInstance_ = 0;

AMCrashMonitorSupport::AMCrashMonitorSupport()
{
	signal(SIGSEGV, handle_signal);
}

AMCrashMonitorSupport* AMCrashMonitorSupport::s(){
	if(!crashMonitorSupportInstance_)
		crashMonitorSupportInstance_ = new AMCrashMonitorSupport();
	return crashMonitorSupportInstance_;
}

qint64 AMCrashMonitorSupport::crashMonitorPID() const{
	return crashMonitorPID_;
}

QFile* AMCrashMonitorSupport::errorFile() const{
	return errorFile_;
}

void AMCrashMonitorSupport::setErrorFile(QFile *errorFile){
	errorFile_ = errorFile;
	globalErrorFile = errorFile;
}

void AMCrashMonitorSupport::monitor(){
	QCoreApplication *app = QApplication::instance();

	QString applicationPath = app->arguments().at(0);
	QFileInfo applicationPathInfo(applicationPath);
	QString applicationRootPath;
	if(applicationPathInfo.isSymLink())
		applicationRootPath = applicationPathInfo.symLinkTarget().section('/', 0, -2);
	else
		applicationRootPath = applicationPathInfo.absoluteDir().path();


	QStringList arguments;
	arguments << "-m";
	arguments << app->applicationFilePath();
	arguments << "/home/acquaman/AcquamanApplicationCrashReports";
	arguments << QString("%1").arg(getpid());

	QProcess::startDetached(applicationRootPath+"/AMCrashReporter", arguments, QDir::currentPath(), &globalCrashMonitorPID);
}

void AMCrashMonitorSupport::report(){
	kill(crashMonitorPID_, SIGUSR2);
}

void handle_signal(int signum){
	void *array[100];
	size_t size;

	size = backtrace(array, 100);
	backtrace_symbols_fd(array, size, globalErrorFile->handle());

	kill(globalCrashMonitorPID, SIGUSR1);
	signal(signum, SIG_DFL);
	kill(getpid(), signum);
}
