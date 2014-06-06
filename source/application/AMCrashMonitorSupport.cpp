#include "AMCrashMonitorSupport.h"

#include <signal.h>
#include <execinfo.h>

#include <QApplication>
#include <QFileInfo>
#include <QProcess>
#include <QDir>

#include <QDebug>

AMCrashMonitorSupport* AMCrashMonitorSupport::crashMonitorSupportInstance_ = 0;

AMCrashMonitorSupport::AMCrashMonitorSupport()
{
	signal(SIGSEGV, handle_signal);

	QFile localErrorFile(QString("/tmp/ErrorFile%1.txt").arg(getpid()));
	localErrorFile.open(QIODevice::WriteOnly | QIODevice::Text);
	errorFile_ = &localErrorFile;
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


	if(applicationRootPath.contains("SGMAcquaman.app"))
		applicationRootPath.replace("SGMAcquaman.app", "AMCrashReporter.app");


	qDebug() << "Going to try this on " << applicationRootPath;

	QStringList arguments;
	arguments << "-m";
	arguments << app->applicationFilePath();
	//arguments << "/home/acquaman/AcquamanApplicationCrashReports";
	arguments << "/Users/chevrid/AcquamanApplicationCrashReports";
	arguments << QString("%1").arg(getpid());
	QProcess::startDetached(applicationRootPath+"/AMCrashReporter", arguments, QDir::currentPath(), &crashMonitorPID_);
}

void AMCrashMonitorSupport::report(){
	kill(crashMonitorPID_, SIGUSR2);
}

void handle_signal(int signum){
	void *array[100];
	size_t size;

	size = backtrace(array, 100);
	backtrace_symbols_fd(array, size, AMCrashMonitorSupport::s()->errorFile()->handle());

	kill(AMCrashMonitorSupport::s()->crashMonitorPID(), SIGUSR1);
	signal(signum, SIG_DFL);
	kill(getpid(), signum);
}
